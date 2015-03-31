#include "camera.h"

#include <iostream>

std::atomic_uint Camera::s_id{0};

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_id(s_id++),
    m_latest_preview(":/testchart.png"),
    m_latest_preview_time(QDateTime::currentDateTimeUtc()),
    m_state(CAMERA_NONE)
{

}

Camera::Camera(gp::Camera* const gp_camera, QObject *parent) :
    QObject(parent),
    m_id(s_id++),
    m_camera(gp_camera),
    m_controllerThread(new QThread()),
    m_controller(new CameraController(gp_camera)),
    m_eventListenerThread(new QThread()),
    m_eventListener(new CameraEventListener(gp_camera)),
    m_latest_preview(":/testchart.png"),
    m_latest_preview_time(QDateTime::currentDateTimeUtc()),
    m_state(CAMERA_INIT)
{
    // Move the camera controller to it's own thread and connect the signals
    m_controller->moveToThread(m_controllerThread);
    connect(m_controller, &CameraController::nameChanged, this, &Camera::c_setName);
    connect(m_controller, &CameraController::newPreviewImage, this, &Camera::c_setPreviewImage);
    connect(m_controller, &CameraController::newImage, this, &Camera::c_setImage);
    connect(m_controller, &CameraController::apertureChoicesChanged, this, &Camera::c_setApertureChoices);
    connect(m_controller, &CameraController::apertureChanged, this, &Camera::c_setApertureIndex);
    connect(m_controller, &CameraController::shutterChoicesChanged, this, &Camera::c_setShutterChoices);
    connect(m_controller, &CameraController::shutterChanged, this, &Camera::c_setShutterIndex);
    connect(m_controller, &CameraController::isoChoicesChanged, this, &Camera::c_setIsoChoices);
    connect(m_controller, &CameraController::isoChanged, this, &Camera::c_setIsoIndex);
    connect(m_controller, &CameraController::previewStarted, this, &Camera::c_previewStarted);
    connect(m_controller, &CameraController::previewStopped, this, &Camera::c_previewStopped);
    connect(m_controller, &CameraController::resetDone, this, &Camera::c_resetDone);
    // connect(m_controllerThread, &QThread::finished, this, &Camera::previewStopped);
    m_controllerThread->start();

    // Move the event listener to it's own thread and connect signals
    m_eventListener->moveToThread(m_eventListenerThread);
    // Start listeing to events as soon as the thread starts
    connect(m_eventListenerThread, &QThread::started, m_eventListener, &CameraEventListener::startListening);
    connect(m_eventListener, &CameraEventListener::apertureChanged, m_controller, &CameraController::readAperture);
    connect(m_eventListener, &CameraEventListener::shutterChanged, m_controller, &CameraController::readShutter);
    connect(m_eventListener, &CameraEventListener::isoChanged, m_controller, &CameraController::readIso);
    connect(m_eventListener, &CameraEventListener::newImageAdded, m_controller, &CameraController::readImage);
    m_eventListenerThread->start();

    // Read this camera's config
    readConfig();
}

Camera::~Camera()
{
    m_state = CAMERA_SHUTDOWN;
    emit stateChanged(m_state);
    stopPreview();
    if (m_controllerThread != nullptr) {
        m_controllerThread->quit();
    }

    if (m_eventListener)
        m_eventListener->stopListening();
    if (m_eventListenerThread)
        m_eventListenerThread->quit();
    if (m_controllerThread) {
        // Wait at most 5 seconds for thread to stop
        if (!m_controllerThread->wait(5 * 1000))
            std::cout << "~Camera(" << m_id << ") timed out" << std::endl;
        else {
            std::cout << "~Camera(" << m_id << ") joined" << std::endl;
        }
    }
    if (m_eventListenerThread) {
        // Wait at most 5 seconds for thread to stop
        if (!m_eventListenerThread->wait(5 * 1000))
            std::cout << "~Camera(" << m_id << ") timed out" << std::endl;
        else {
            std::cout << "~Camera(" << m_id << ") joined" << std::endl;
        }
    }
    delete m_controller;
    delete m_controllerThread;
    delete m_eventListener;
    delete m_eventListenerThread;
}

void Camera::readConfig() {
    if (m_controller != nullptr)
        QMetaObject::invokeMethod(m_controller, "readConfig", Qt::QueuedConnection);
}

QString Camera::name() const {
    return m_name;
}

void Camera::c_setName(const QString& name) {
    m_name = name;
    if (m_state == CAMERA_INIT) {
        startPreview();
    }
    emit nameChanged(m_name);
}

void Camera::c_previewStarted() {
    m_state = CAMERA_PREVIEW;
    emit stateChanged(m_state);
}

void Camera::c_previewStopped() {
    m_latest_preview = QImage(":/testchart.png");
    m_latest_preview_time = QDateTime::currentDateTimeUtc();
    emit previewUrlChanged(previewUrl());
    if (m_state != CAMERA_SHUTDOWN) {
        m_state = CAMERA_TRANSITIONING;
        emit stateChanged(m_state);
        reset(); // release UI lock
    }
}

void Camera::reset() {
    if (m_controller)
        QMetaObject::invokeMethod(m_controller, "reset", Qt::QueuedConnection);
}

void Camera::trigger() {
    if (m_state != CAMERA_CAPTURE)
        return;
    QMetaObject::invokeMethod(m_controller, "trigger", Qt::QueuedConnection);
}

void Camera::c_resetDone() {
    if (m_state != CAMERA_SHUTDOWN) {
        m_state = CAMERA_CAPTURE;
        emit stateChanged(m_state);
    }
}

void Camera::setState(const CameraState state) {
    if (state == CAMERA_PREVIEW && m_state != CAMERA_PREVIEW) {
        startPreview();
    }
    else if (state == CAMERA_CAPTURE && m_state != CAMERA_CAPTURE) {
        stopPreview();
    }
    else if (state != m_state) {
        std::cout << "Camera " << m_id << ": can't set state to " << state << std::endl;
    }
}

QString Camera::aperture() const {
    if (m_aperture != -1)
        return m_apertureChoices.at(m_aperture);
    return QString();
}

QString Camera::shutter() const {
    if (m_shutter != -1)
        return m_shutterChoices.at(m_shutter);
    return QString();
}

QString Camera::iso() const {
    if (m_iso != -1)
        return m_isoChoices.at(m_iso);
    return QString();
}

int Camera::apertureIndex() const {
    return m_aperture;
}

int Camera::shutterIndex() const {
    return m_shutter;
}

int Camera::isoIndex() const {
    return m_iso;
}

void Camera::setApertureIndex(const int index) {
    if (index >= 0 && index < m_apertureChoices.size())
        QMetaObject::invokeMethod(m_controller, "setAperture", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::setShutterIndex(const int index) {
    if (index >= 0 && index < m_shutterChoices.size())
        QMetaObject::invokeMethod(m_controller, "setShutter", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::setIsoIndex(const int index) {
    if (index >= 0 && index < m_isoChoices.size())
        QMetaObject::invokeMethod(m_controller, "setIso", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::c_setApertureIndex(const int aperture) {
    if (aperture >= 0 && aperture < m_apertureChoices.size())
        m_aperture = aperture;
    else
        m_aperture = -1;
    emit apertureChanged(this->aperture());
    emit apertureIndexChanged(m_aperture);
}

void Camera::c_setShutterIndex(const int shutter) {
    if (shutter >= 0 && shutter < m_shutterChoices.size())
        m_shutter = shutter;
    else
        m_shutter = -1;
    emit shutterChanged(this->shutter());
    emit shutterIndexChanged(m_shutter);
}

void Camera::c_setIsoIndex(const int iso) {
    if (iso >= 0 && iso < m_isoChoices.size())
        m_iso = iso;
    else
        m_iso = -1;
    emit isoChanged(this->iso());
    emit isoIndexChanged(m_iso);
}

QStringList Camera::apertureChoices() const {
    return m_apertureChoices;
}

QStringList Camera::shutterChoices() const {
    return m_shutterChoices;
}

QStringList Camera::isoChoices() const {
    return m_isoChoices;
}

void Camera::c_setApertureChoices(const QList<QString>& newApertureChoices) {
    m_apertureChoices = newApertureChoices;
    if (m_aperture >= m_apertureChoices.size()) {
        m_aperture = m_apertureChoices.size() == 0 ? -1 : 0;
    }
    emit apertureChoicesChanged(m_apertureChoices);
    emit apertureChanged(aperture());
    emit apertureIndexChanged(m_aperture);
}

void Camera::c_setShutterChoices(const QList<QString>& newShutterChoices) {
    m_shutterChoices = newShutterChoices;
    if (m_shutter >= m_shutterChoices.size()) {
        m_shutter = m_shutterChoices.size() == 0 ? -1 : 0;
        emit shutterChanged(shutter());
    }
    emit shutterChoicesChanged(m_shutterChoices);
    emit shutterChanged(shutter());
    emit shutterIndexChanged(m_shutter);
}

void Camera::c_setIsoChoices(const QList<QString>& newIsoChoices) {
    m_isoChoices = newIsoChoices;
    if (m_iso >= m_isoChoices.size()) {
        m_iso = m_isoChoices.size() == 0 ? -1 : 0;
        emit isoChanged(iso());
    }
    emit isoChoicesChanged(m_isoChoices);
    emit isoChanged(iso());
    emit isoIndexChanged(m_iso);
}

void Camera::startPreview() {
    if (m_controller != nullptr)
        QMetaObject::invokeMethod(m_controller, "startPreview", Qt::QueuedConnection);
}

void Camera::stopPreview() {
    if (m_controller != nullptr)
        m_controller->stopPreview();
}

void Camera::c_setPreviewImage(const QImage preview) {
    if (m_state != CAMERA_PREVIEW)
        return;
    m_latest_preview = preview;
    m_latest_preview_time = QDateTime::currentDateTimeUtc();
    emit previewUrlChanged(previewUrl());
}

void Camera::c_setImage(const Image& image) {
    if (m_state != CAMERA_CAPTURE)
        return;
    m_latest_image = image;
    m_latest_image_time = QDateTime::currentDateTimeUtc();
    emit imageUrlChanged(imageUrl());
}

void Camera::clearLatestImage() {
    m_latest_image = Image();
    m_latest_image_time = QDateTime::currentDateTimeUtc();
    emit imageUrlChanged(imageUrl());
}

QString Camera::previewUrl() const {
    return QString::number(m_id) + QString("/preview/") + m_latest_preview_time.toString("dd.MM.yyyy-hh:mm:ss.zzz");
}

QString Camera::imageUrl() const {
    return QString::number(m_id) + QString("/image/") + m_latest_image_time.toString("dd.MM.yyyy-hh:mm:ss.zzz");
}

const QImage& Camera::latestPreview() const {
    return m_latest_preview;
}

const QImage& Camera::latestImage() const {
    return m_latest_image.toQImage();
}

Camera::CameraState Camera::state() const {
    return m_state;
}

void Camera::saveImage(const QString& fileName) {
    m_latest_image.save(fileName.toStdString());
}
