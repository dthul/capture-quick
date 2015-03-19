#include "camera.h"

Camera::Camera(QObject *parent) : QObject(parent)
{

}

Camera::Camera(gp::Camera* const gp_camera, QObject *parent) :
    QObject(parent),
    m_camera(gp_camera)
{

}

Camera::~Camera()
{

}

void Camera::readConfig() {
    // Clear the old settings
    m_aperture = -1;
    m_apertureChoices.clear();
    m_shutter = -1;
    m_shutterChoices.clear();
    m_iso = -1;
    m_isoChoices.clear();

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        gp::Aperture apertureConfig = configWidget.get<gp::Aperture>();
        gp::ShutterSpeed shutterConfig = configWidget.get<gp::ShutterSpeed>();
        gp::Iso isoConfig = configWidget.get<gp::Iso>();

        for (const auto& apertureOption : apertureConfig.choices())
            m_apertureChoices.append(QString::fromStdString(apertureOption));
        m_aperture = apertureConfig.index();

        for (const auto& shutterOption : shutterConfig.choices())
            m_shutterChoices.append(QString::fromStdString(shutterOption));
        m_shutter = shutterConfig.index();

        for (const auto& isoOption : isoConfig.choices())
            m_isoChoices.append(QString::fromStdString(isoOption));
        m_iso = isoConfig.index();
    }

    // Notify all observers about the change in settings
    emit apertureChanged(aperture());
    emit apertureChoicesChanged(m_apertureChoices);
    emit shutterChanged(shutter());
    emit shutterChoicesChanged(m_shutterChoices);
    emit isoChanged(iso());
    emit isoChoicesChanged(m_isoChoices);
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

QList<QString> Camera::apertureChoices() const {
    return m_apertureChoices;
}

QList<QString> Camera::shutterChoices() const {
    return m_shutterChoices;
}

QList<QString> Camera::isoChoices() const {
    return m_isoChoices;
}

void Camera::startPreview() {
    if (m_previewThread != nullptr)
        return;
    m_previewThread = new QThread;
    m_previewFeed = new PreviewFeed(m_camera);
    m_previewFeed->moveToThread(m_previewThread);
    connect(m_previewThread, &QThread::started, m_previewFeed, &PreviewFeed::start);
    connect(m_previewFeed, &PreviewFeed::stopped, m_previewThread, &QThread::quit);
    connect(m_previewThread, &QThread::finished, this, &Camera::previewStopped);
    m_previewThread->start();
}

void Camera::stopPreview() {
    if (m_previewFeed != nullptr) {
        // previewFeed->stop();
        QMetaObject::invokeMethod(m_previewFeed, "stop", Qt::QueuedConnection);
    }
}

void Camera::previewStopped() {
    delete m_previewFeed;
    m_previewFeed = nullptr;
    delete m_previewThread;
    m_previewThread = nullptr;
}
