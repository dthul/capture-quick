#include "camera.h"

#include <iostream>

std::atomic_uint Camera::s_id{0};

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_id(s_id++),
    m_state(CAMERA_NONE)
{

}

Camera::Camera(gp::Camera* const gp_camera, QObject *parent) :
    QObject(parent),
    m_id(s_id++),
    m_camera(gp_camera),
    m_controllerThread(new QThread()),
    m_controller(new CameraController(gp_camera)),
    m_state(CAMERA_INIT)
{
    // Move the camera controller to it's own thread and connect the signals
    m_controller->moveToThread(m_controllerThread);
    connect(m_controller, &CameraController::newPreviewImage, this, &Camera::setPreviewImage);
    connect(m_controller, &CameraController::apertureChoicesChanged, this, &Camera::setApertureChoices);
    connect(m_controller, &CameraController::apertureChanged, this, &Camera::setApertureIndex);
    connect(m_controller, &CameraController::shutterChoicesChanged, this, &Camera::setShutterChoices);
    connect(m_controller, &CameraController::shutterChanged, this, &Camera::setShutterIndex);
    connect(m_controller, &CameraController::isoChoicesChanged, this, &Camera::setIsoChoices);
    connect(m_controller, &CameraController::isoChanged, this, &Camera::setIsoIndex);
    connect(m_controllerThread, &QThread::finished, this, &Camera::previewStopped);
    m_controllerThread->start();

    // Read this camera's config
    readConfig();
}

Camera::~Camera()
{
    m_state = CAMERA_SHUTDOWN;
    stopPreview();
    if (m_controllerThread != nullptr) {
        m_controllerThread->quit();
        // Wait at most 10 seconds for preview threads to stop
        if (!m_controllerThread->wait(10 * 1000))
            std::cout << "~Camera(" << m_id << ") timed out" << std::endl;
        else {
            std::cout << "~Camera(" << m_id << ") joined" << std::endl;
        }
    }
}

void Camera::readConfig() {
    if (m_controller != nullptr)
        QMetaObject::invokeMethod(m_controller, "readConfig", Qt::QueuedConnection);
}

QString Camera::aperture() const {
    if (m_aperture != -1)
        return m_apertureChoices.at(m_aperture);
    return QString();
}

void Camera::setAperture(const QString &aperture) {
    for (auto i = 0; i < m_apertureChoices.size(); ++i) {
        if (aperture == m_apertureChoices.at(i)) {
            m_aperture = i;
            emit apertureChanged(this->aperture());
            return;
        }
    }
}

QString Camera::shutter() const {
    if (m_shutter != -1)
        return m_shutterChoices.at(m_shutter);
    return QString();
}

void Camera::setShutter(const QString &shutter) {
    for (auto i = 0; i < m_shutterChoices.size(); ++i) {
        if (shutter == m_shutterChoices.at(i)) {
            m_shutter = i;
            emit shutterChanged(this->shutter());
            return;
        }
    }
}

QString Camera::iso() const {
    if (m_iso != -1)
        return m_isoChoices.at(m_iso);
    return QString();
}

void Camera::setIso(const QString &iso) {
    for (auto i = 0; i < m_isoChoices.size(); ++i) {
        if (iso == m_isoChoices.at(i)) {
            m_iso = i;
            emit isoChanged(this->iso());
            return;
        }
    }
}

void Camera::setApertureIndex(const int aperture) {
    if (aperture >= 0 && aperture < m_apertureChoices.size())
        m_aperture = aperture;
    else
        m_aperture = -1;
    emit apertureChanged(aperture());
}

void Camera::setShutterIndex(const int shutter) {
    if (shutter >= 0 && shutter < m_shutterChoices.size())
        m_shutter = shutter;
    else
        m_shutter = -1;
    emit shutterChanged(shutter());
}

void Camera::setIsoIndex(const int iso) {
    if (iso >= 0 && iso < m_isoChoices.size())
        m_iso = iso;
    else
        m_iso = -1;
    emit isoChanged(iso());
}

QList<QString> Camera::apertureChoices() const {
    return m_apertureChoices;
}

QList<QString> Camera::shutterChoices() const {
    return m_shutterChoices;
}

QList<QString> Camera::isoChoices() const {
    return m_isoChoices;
}

void Camera::setApertureChoices(const QList<QString>& newApertureChoices) {
    m_apertureChoices = newApertureChoices;
    if (m_aperture >= m_apertureChoices.size()) {
        m_aperture = m_apertureChoices.size() == 0 ? -1 : 0;
        emit apertureChanged(m_aperture);
    }
    emit apertureChoicesChanged(m_apertureChoices);
}

void Camera::setShutterChoices(const QList<QString>& newShutterChoices) {
    m_shutterChoices = newShutterChoices;
    if (m_shutter >= m_shutterChoices.size()) {
        m_shutter = m_shutterChoices.size() == 0 ? -1 : 0;
        emit shutterChanged(m_shutter);
    }
    emit shutterChoicesChanged(m_shutterChoices);
}

void Camera::setIsoChoices(const QList<QString>& newIsoChoices) {
    m_isoChoices = newIsoChoices;
    if (m_iso >= m_isoChoices.size()) {
        m_iso = m_isoChoices.size() == 0 ? -1 : 0;
        emit isoChanged(m_iso);
    }
    emit isoChoicesChanged(m_isoChoices);
}

void Camera::startPreview() {
    if (m_controller != nullptr)
        QMetaObject::invokeMethod(m_controller, "startPreview", Qt::QueuedConnection);
}

void Camera::stopPreview() {
    if (m_controller != nullptr)
        m_controller->stopPreview();
}

void Camera::setPreviewImage(const QImage preview) {
    m_latest_preview = preview;
    m_latest_preview_time = QDateTime::currentDateTimeUtc();
    std::cout << "emitting preview url changed" << std::endl;
    emit previewUrlChanged(previewUrl());
}

QString Camera::previewUrl() const {
    return QString::number(m_id) + QString("/") + m_latest_preview_time.toString("dd.MM.yyyy-hh:mm:ss.zzz");
}

const QImage& Camera::latestPreview() const {
    return m_latest_preview;
}
