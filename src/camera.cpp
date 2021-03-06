#include "camera.h"

#include <iostream>

#include "cameracontroller.h"

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_latest_preview(new Image(":/testchart.png")),
    m_latest_image(new Image()),
    m_latest_raw_image(new Image()),
    m_state(CAMERA_NONE)
{

}

Camera::Camera(std::shared_ptr<gp::Camera> const gp_camera, QObject *parent) :
    QObject(parent),
    m_camera(gp_camera),
    m_controllerThread(new QThread()),
    m_eventListenerThread(new QThread()),
    m_eventListener(new CameraEventListener(gp_camera)),
    m_latest_preview(new Image(":/testchart.png")),
    m_latest_image(new Image()),
    m_latest_raw_image(new Image()),
    m_state(CAMERA_INIT)
{
    m_controller = new CameraController(this);
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
    connect(m_controller, &CameraController::viewfinderChanged, this, &Camera::c_viewfinderChanged);
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
    // readConfig();
    // Let Capture call this function so that it has time to connect its
    // slots to our signals before the config is read
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
            std::cout << "~Camera(" << m_name.toStdString() << ") timed out while waiting for controller thread" << std::endl;
    }
    if (m_eventListenerThread) {
        // Wait at most 5 seconds for thread to stop
        if (!m_eventListenerThread->wait(5 * 1000))
            std::cout << "~Camera(" << m_name.toStdString() << ") timed out while waiting for event listener thread" << std::endl;
    }
    delete m_controller;
    delete m_controllerThread;
    delete m_eventListener;
    delete m_eventListenerThread;
}

std::shared_ptr<gp::Camera> Camera::gp_camera() {
    return std::shared_ptr<gp::Camera>(m_camera);
}

void Camera::readConfig() {
    if (m_controller != nullptr)
        QMetaObject::invokeMethod(m_controller, "readConfig", Qt::QueuedConnection);
}

QString Camera::name() const {
    return m_name;
}

void Camera::c_setName(const QString& name) {
    if (name == m_name)
        return;
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
    // Make sure that the signal is emitted before the old Image is deleted
    QSharedPointer<Image> tmp(m_latest_preview);
    m_latest_preview = QSharedPointer<Image>(new Image(":/testchart.png"));
    emit previewChanged(m_latest_preview.data());
    if (m_state != CAMERA_SHUTDOWN) {
        m_state = CAMERA_TRANSITIONING;
        emit stateChanged(m_state);
        // reset(); // release UI lock
        setViewfinder(true);
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

void Camera::setViewfinder(bool on) {
    QMetaObject::invokeMethod(m_controller, "setViewfinder", Qt::QueuedConnection, Q_ARG(bool, on));
}

void Camera::c_resetDone() {
    if (m_state != CAMERA_SHUTDOWN) {
        m_state = CAMERA_CAPTURE;
        emit stateChanged(m_state);
    }
}

void Camera::c_viewfinderChanged(const bool on) {
    if (m_state != CAMERA_SHUTDOWN) {
        if (on && m_state == CAMERA_TRANSITIONING) {
            m_state = CAMERA_CAPTURE;
            emit stateChanged(m_state);
        }
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
        std::cout << "Camera " << m_name.toStdString() << ": can't set state to " << state << std::endl;
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
    if (index >= 0 && index < m_apertureChoices.size() && index != m_aperture)
        QMetaObject::invokeMethod(m_controller, "setAperture", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::setShutterIndex(const int index) {
    if (index >= 0 && index < m_shutterChoices.size() && index != m_shutter)
        QMetaObject::invokeMethod(m_controller, "setShutter", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::setIsoIndex(const int index) {
    if (index >= 0 && index < m_isoChoices.size() && index != m_iso)
        QMetaObject::invokeMethod(m_controller, "setIso", Qt::QueuedConnection, Q_ARG(int, index));
}

void Camera::c_setApertureIndex(const int aperture) {
    if (aperture == m_aperture)
        return;
    if (aperture >= 0 && aperture < m_apertureChoices.size())
        m_aperture = aperture;
    else
        m_aperture = -1;
    emit apertureChanged(this->aperture());
    emit apertureIndexChanged(m_aperture);
}

void Camera::c_setShutterIndex(const int shutter) {
    if (shutter == m_shutter)
        return;
    if (shutter >= 0 && shutter < m_shutterChoices.size())
        m_shutter = shutter;
    else
        m_shutter = -1;
    emit shutterChanged(this->shutter());
    emit shutterIndexChanged(m_shutter);
}

void Camera::c_setIsoIndex(const int iso) {
    if (iso == m_iso)
        return;
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
    if (newApertureChoices == m_apertureChoices)
        return;
    m_apertureChoices = newApertureChoices;
    if (m_aperture >= m_apertureChoices.size()) {
        m_aperture = m_apertureChoices.size() == 0 ? -1 : 0;
    }
    emit apertureChoicesChanged(m_apertureChoices);
    emit apertureChanged(aperture());
    emit apertureIndexChanged(m_aperture);
}

void Camera::c_setShutterChoices(const QList<QString>& newShutterChoices) {
    if (newShutterChoices == m_shutterChoices)
        return;
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
    if (newIsoChoices == m_isoChoices)
        return;
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

void Camera::c_setPreviewImage(QSharedPointer<Image> preview) {
    if (m_state != CAMERA_PREVIEW)
        return;
    // Make sure that the signal is emitted before the old Image is deleted
    QSharedPointer<Image> tmp(m_latest_preview);
    m_latest_preview = preview;
    emit previewChanged(m_latest_preview.data());
}

void Camera::c_setImage(QSharedPointer<Image> image) {
    if (m_state != CAMERA_CAPTURE)
        return;
    if (image->is_raw()) {
        // Make sure that the signal is emitted before the old Image is deleted
        QSharedPointer<Image> tmp(m_latest_raw_image);
        m_latest_raw_image = image;
        emit rawImageChanged(m_latest_raw_image.data());
    }
    else {
        // Make sure that the signal is emitted before the old Image is deleted
        QSharedPointer<Image> tmp(m_latest_image);
        m_latest_image = image;
        emit imageChanged(m_latest_image.data());
    }
}

void Camera::clearLatestImage() {
    // Make sure that the signal is emitted before the old Image is deleted
    QSharedPointer<Image> tmp1(m_latest_image);
    QSharedPointer<Image> tmp2(m_latest_raw_image);
    m_latest_image = QSharedPointer<Image>(new Image());
    m_latest_raw_image = QSharedPointer<Image>(new Image());
    emit imageChanged(m_latest_image.data());
    emit rawImageChanged(m_latest_raw_image.data());
}

Image* Camera::preview() {
    return m_latest_preview.data();
}

Image* Camera::image() {
    return m_latest_image.data();
}

Image* Camera::rawImage() {
    return m_latest_raw_image.data();
}

Camera::CameraState Camera::state() const {
    return m_state;
}
