#include "capture.h"

#include <iostream>

#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>

Capture::Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent) :
    QObject(parent),
    m_qml_engine(qmlEngine),
    m_num_captured(0),
    m_capture_round(0)
{
    QSettings settings;
    m_capture_root = settings.value("capture/root_path", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    m_gp_cameras = gpcontext.all_cameras();
    std::cout << "Found " << m_gp_cameras.size() << " cameras" << std::endl;
    for (auto& gp_camera : m_gp_cameras) {
        Camera* camera = new Camera(&gp_camera);
        m_cameras.append(camera);
        connect(camera, &Camera::imageUrlChanged, this, &Capture::newImageCaptured);
    }

    // will be freed by Qt
    LiveImageProvider *liveImgProvider = new LiveImageProvider(reinterpret_cast<QList<Camera*>*>(&m_cameras));
    m_qml_engine->addImageProvider("live", liveImgProvider);

    m_qml_engine->rootContext()->setContextProperty("cameras", QVariant::fromValue(m_cameras));
    m_qml_engine->rootContext()->setContextProperty("capture", this);
}

Capture::~Capture()
{
    for (QObject* camera : m_cameras) {
        delete camera;
    }
    m_cameras.clear();
    // This is the call that actually releases the cameras
    // by destroying the underlying gp::Cameras
    m_gp_cameras.clear();
}

int Capture::numCaptured() const {
    return m_num_captured;
}

void Capture::newCapture() {
    for (QObject* camera : m_cameras)
        reinterpret_cast<Camera*>(camera)->clearLatestImage();
}

void Capture::saveCaptureToDisk() {
    for (QObject* qamera : m_cameras) {
        Camera* camera = reinterpret_cast<Camera*>(qamera);
        if (!camera->latestImage().isNull())
            camera->saveImage("capture.jpg"); // TODO: implement structuring schemes
    }
}

void Capture::newImageCaptured() {
    int num_captured = 0;
    for (QObject* qamera : m_cameras) {
        Camera* camera = reinterpret_cast<Camera*>(qamera);
        if (!camera->latestImage().isNull())
            ++num_captured;
    }
    m_num_captured = num_captured;
    emit numCapturedChanged(m_num_captured);
}

QString Capture::captureRoot() const {
    return m_capture_root;
}

void Capture::setCaptureRoot(const QString& newCaptureRoot) {
    QSettings settings;
    m_capture_root = newCaptureRoot;
    settings.setValue("capture/root_path", newCaptureRoot);
    emit captureRootChanged(m_capture_root);
}
