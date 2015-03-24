#include "cameracontroller.h"

#include <QMetaObject>

CameraController::CameraController(gp::Camera *camera, QObject *parent) :
    QObject(parent),
    m_camera(camera),
    m_previewRunning(false)
{

}

CameraController::~CameraController()
{

}

void CameraController::startPreview() {
    m_previewRunning = true;
    QMetaObject::invokeMethod(this, "capturePreview", Qt::QueuedConnection);
    emit previewStarted();
}

void CameraController::stopPreview() {
    m_previewRunning = false;
    emit previewStopped();
}

void CameraController::readConfig() const {
    int aperture = -1;
    std::vector<QString> apertureChoices;
    int shutter = -1;
    std::vector<QString> shutterChoices;
    int iso = -1;
    std::vector<QString> isoChoices;

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        gp::Aperture apertureConfig = configWidget["aperture"].get<gp::Aperture>();
        gp::ShutterSpeed shutterConfig = configWidget["shutterspeed"].get<gp::ShutterSpeed>();
        gp::Iso isoConfig = configWidget["iso"].get<gp::Iso>();

        for (const auto& apertureOption : apertureConfig.choices())
            apertureChoices.append(QString::fromStdString(apertureOption));
        aperture = apertureConfig.index();

        for (const auto& shutterOption : shutterConfig.choices())
            shutterChoices.append(QString::fromStdString(shutterOption));
        shutter = shutterConfig.index();

        for (const auto& isoOption : isoConfig.choices())
            isoChoices.append(QString::fromStdString(isoOption));
        iso = isoConfig.index();
    }

    // Notify all observers about the change in settings
    emit apertureChoicesChanged(apertureChoices);
    emit apertureChanged(aperture);
    emit shutterChoicesChanged(shutterChoices);
    emit shutterChanged(shutter);
    emit isoChoicesChanged(isoChoices);
    emit isoChanged(iso);
}

void CameraController::capturePreview() const {
    if (!m_previewRunning)
        return;
    JpegBuffer jpeg;
    try {
        std::cout << "Waiting for preview..." << std::endl;
        jpeg = m_camera->preview();
        if (m_previewRunning) {
            // Convert the buffer containing jpeg data to a QImage
            QImage previewImage(QImage::fromData(reinterpret_cast<const uchar*>(jpeg.data()), jpeg.size()));
            std::cout << "emitting new preview image" << std::endl;
            emit newPreviewImage(previewImage);
        }
    } catch (gp::Exception& ex) {
        std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
    }
    if (m_previewRunning) // queue new preview capture
        QMetaObject::invokeMethod(this, "capturePreview", Qt::QueuedConnection);
}
