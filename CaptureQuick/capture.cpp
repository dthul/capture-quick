#include "capture.h"

#include <iostream>

#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>

#include "persist.h"

Capture::Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent) :
    QObject(parent),
    m_qml_engine(qmlEngine),
    m_num_captured(0)
{
    QSettings settings;
    const QString defaultCaptureLocation =
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    m_capture_root = settings.value("capture/root_path", defaultCaptureLocation).toString();
    m_auto_save = settings.value("capture/auto_save", false).toBool();
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
    for (auto qcamera : m_cameras) {
        delete qcamera;
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
    m_capture_time = QDateTime::currentDateTimeUtc();
    for (auto qcamera : m_cameras)
        reinterpret_cast<Camera*>(qcamera)->clearLatestImage();
}

void Capture::saveCaptureToDisk() {
    std::cout << "Saving capture to disk" << std::endl;
    Persist::saveImagesToDisk(*reinterpret_cast<QList<Camera*>*>(&m_cameras), QString::number(m_capture_time.toMSecsSinceEpoch()));
}

void Capture::newImageCaptured() {
    int num_captured = 0;
    for (auto qamera : m_cameras) {
        Camera* camera = reinterpret_cast<Camera*>(qamera);
        if (!camera->latestImage().isNull())
            ++num_captured;
    }
    m_num_captured = num_captured;
    emit numCapturedChanged(m_num_captured);
    if (m_auto_save && m_num_captured == m_cameras.length())
        saveCaptureToDisk();
}

QString Capture::captureRoot() const {
    return m_capture_root;
}

void Capture::setCaptureRoot(const QString& newCaptureRoot) {
    QSettings settings;
    m_capture_root = newCaptureRoot;
    settings.setValue("capture/root_path", m_capture_root);
    emit captureRootChanged(m_capture_root);
}

bool Capture::autoSave() const {
    return m_auto_save;
}

void Capture::setAutoSave(bool autoSave) {
    QSettings settings;
    m_auto_save = autoSave;
    settings.setValue("capture/auto_save", m_auto_save);
    emit autoSaveChanged(m_auto_save);
}
