#include "cameracontroller.h"

#include <iostream>

#include <QImage>
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

void CameraController::readConfig() {
    int aperture = -1;
    QStringList apertureChoices;
    int shutter = -1;
    QStringList shutterChoices;
    int iso = -1;
    QStringList isoChoices;

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        std::string name = configWidget["artist"].get<std::string>();
        gp::Aperture apertureConfig = configWidget["aperture"].get<gp::Aperture>();
        gp::ShutterSpeed shutterConfig = configWidget["shutterspeed"].get<gp::ShutterSpeed>();
        gp::Iso isoConfig = configWidget["iso"].get<gp::Iso>();

        emit nameChanged(QString::fromStdString(name));

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

void CameraController::readAperture() {
    int aperture = -1;

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        gp::Aperture apertureConfig = configWidget["aperture"].get<gp::Aperture>();
        aperture = apertureConfig.index();
    }

    // Notify all observers about the change in settings
    emit apertureChanged(aperture);;
}

void CameraController::readShutter() {
    int shutter = -1;

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        gp::ShutterSpeed shutterConfig = configWidget["shutterspeed"].get<gp::ShutterSpeed>();
        shutter = shutterConfig.index();
    }

    // Notify all observers about the change in settings
    emit shutterChanged(shutter);;
}

void CameraController::readIso() {
    int iso = -1;

    if (m_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_camera->config();
        gp::Iso isoConfig = configWidget["iso"].get<gp::Iso>();
        iso = isoConfig.index();
    }

    // Notify all observers about the change in settings
    emit isoChanged(iso);;
}

void CameraController::capturePreview() {
    if (!m_previewRunning)
        return;
    try {
        JpegBuffer jpeg = m_camera->preview();
        if (m_previewRunning) {
            // Convert the buffer containing jpeg data to a QImage
            QImage previewImage(QImage::fromData(reinterpret_cast<const uchar*>(jpeg.data()), jpeg.size()));
            emit newPreviewImage(previewImage);
        }
    } catch (gp::Exception& ex) {
        std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
    }
    if (m_previewRunning) // queue new preview capture
        QMetaObject::invokeMethod(this, "capturePreview", Qt::QueuedConnection);
}

void CameraController::readImage(const QFileInfo& fileInfo) {
    try {
        JpegBuffer jpeg = m_camera->read_image(fileInfo.path().toStdString(), fileInfo.fileName().toStdString());
        Image image(jpeg);
        emit newImage(image);
    } catch (gp::Exception& ex) {
        std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
    }
}

template <class Obj>
void CameraController::setRadioConfig(int value) {
    auto cfg = m_camera->config()[Obj::gpname];
    auto radio = cfg.template get<Obj>();

    if (value >= 0 && value < radio.size()) {
        radio.set(value);
        cfg.set(radio);
        std::cout << " new " << Obj::gpname << " " << radio.text() << std::endl;
    }
}

void CameraController::setAperture(const int index) {
    setRadioConfig<gp::Aperture>(index);
    // Strictly speaking we don't need to call
    // readAperture() since the event listener
    // will notice the change and trigger a read.
    // But especially shortly after the program
    // has started it will take a long time for this
    // event to come through so update it immediately
    // instead.
    readAperture();
}

void CameraController::setShutter(const int index) {
    setRadioConfig<gp::ShutterSpeed>(index);
    // Strictly speaking we don't need to call
    // readShutter() since the event listener
    // will notice the change and trigger a read.
    // But especially shortly after the program
    // has started it will take a long time for this
    // event to come through so update it immediately
    // instead.
    readShutter();
}

void CameraController::setIso(const int index) {
    setRadioConfig<gp::Iso>(index);
    // Strictly speaking we don't need to call
    // readIso() since the event listener
    // will notice the change and trigger a read.
    // But especially shortly after the program
    // has started it will take a long time for this
    // event to come through so update it immediately
    // instead.
    readIso();
}

void CameraController::reset() {
    m_camera->reset();
    emit resetDone();
}
