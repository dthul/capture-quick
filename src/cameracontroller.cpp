#include "cameracontroller.h"

#include <iostream>

#include <QApplication>
#include <QImage>
#include <QMetaObject>
#include <QSettings>

CameraController::CameraController(Camera * const camera, QObject *parent) :
    QObject(parent),
    m_camera(camera),
    m_gp_camera(camera->gp_camera()),
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

    if (m_gp_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_gp_camera->config();
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

    //gp::Camera::FileFunc func = [](const gp::Camera::FileInfo& info) -> void { std::cout << info.name << std::endl; };
    //m_gp_camera->for_each_file(func);
}

void CameraController::readAperture() {
    int aperture = -1;

    if (m_gp_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_gp_camera->config();
        gp::Aperture apertureConfig = configWidget["aperture"].get<gp::Aperture>();
        aperture = apertureConfig.index();
    }

    // Notify all observers about the change in settings
    emit apertureChanged(aperture);
}

void CameraController::readShutter() {
    int shutter = -1;

    if (m_gp_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_gp_camera->config();
        gp::ShutterSpeed shutterConfig = configWidget["shutterspeed"].get<gp::ShutterSpeed>();
        shutter = shutterConfig.index();
    }

    // Notify all observers about the change in settings
    emit shutterChanged(shutter);
}

void CameraController::readIso() {
    int iso = -1;

    if (m_gp_camera != nullptr) {
        // Read the new settings from the camera
        const gp::Widget configWidget = m_gp_camera->config();
        gp::Iso isoConfig = configWidget["iso"].get<gp::Iso>();
        iso = isoConfig.index();
    }

    // Notify all observers about the change in settings
    emit isoChanged(iso);
}

void CameraController::capturePreview() {
    if (!m_previewRunning)
        return;
    try {
        auto jpeg = m_gp_camera->preview();
        if (m_previewRunning) {
            QSharedPointer<Image> previewImage(new Image(jpeg, m_camera));
            previewImage.data()->moveToThread(QApplication::instance()->thread());
            emit newPreviewImage(previewImage);
        }
    } catch (gp::Exception& ex) {
        std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
    }
    if (m_previewRunning) // queue new preview capture
        QMetaObject::invokeMethod(this, "capturePreview", Qt::QueuedConnection);
}

void CameraController::trigger() {
    try {
        m_gp_camera->trigger();
    } catch (gp::Exception& ex) {
        std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
    }
}

void CameraController::readImage(const QFileInfo& fileInfo) {
    try {
        std::cout << fileInfo.path().toStdString() << fileInfo.fileName().toStdString() << std::endl;
        if (fileInfo.fileName().endsWith(".cr2")) {
            // I don't know if this is the best place for this test
            QSettings settings;
            const bool save_raw = settings.value("capture/save_raw", false).toBool();
            if (!save_raw) {
                // Don't waste bandwidth by not downloading the raw image in the first place.
                // Just return an empty image.
                const std::vector<char> buffer;
                QSharedPointer<Image> image(new Image(buffer, m_camera));
                image.data()->moveToThread(QApplication::instance()->thread());
                emit newImage(image);
                return;
            }
        }
        std::vector<char> image_data = m_gp_camera->read_image(fileInfo.path().toStdString(), fileInfo.fileName().toStdString());
        QSharedPointer<Image> image(new Image(image_data, m_camera));
        image.data()->moveToThread(QApplication::instance()->thread());
        emit newImage(image);
    } catch (gp::Exception& ex) {
        std::cout << "cam " << m_camera->name().toStdString() << ": " << ex.what() << std::endl;
    }
}

template <class Obj>
void CameraController::setRadioConfig(int value) {
    auto cfg = m_gp_camera->config()[Obj::gpname];
    auto radio = cfg.template get<Obj>();

    if (value >= 0 && value < radio.size()) {
        radio.set(value);
        cfg.set(radio);
        std::cout << " new " << Obj::gpname << " " << radio.text() << std::endl;
    }
}

void CameraController::setViewfinder(bool on) {
    auto cfg = m_gp_camera->config()["viewfinder"];
    cfg.set<bool>(!on);
    emit viewfinderChanged(on);
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
    m_gp_camera->reset();
    emit resetDone();
}
