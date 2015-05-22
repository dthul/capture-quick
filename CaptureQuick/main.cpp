#ifdef __APPLE__
#include <cstdlib>
#endif
#include <fstream>
#include <iostream>

#include <QApplication>
#include <QFileInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "camera.h"
#include "capture.h"
#include "image.h"
#include "liveimageprovider.h"
#include "util.h"

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    {
        const QFileInfo executable_path(QString::fromStdString(util::executable_path()));
        QDir iolib_dir = executable_path.absoluteDir();
        if (iolib_dir.cd("../Frameworks/libgphoto2_port/0.12.0/")) {
            std::cout << "IOLIBS" << iolib_dir.absolutePath().toStdString() << std::endl;
            util::setenv("IOLIBS", iolib_dir.absolutePath().toStdString());
        }
        QDir camlib_dir = executable_path.absoluteDir();
        if (camlib_dir.cd("../Frameworks/libgphoto2/2.5.7/")) {
            std::cout << "CAMLIBS" << camlib_dir.absolutePath().toStdString() << std::endl;
            util::setenv("CAMLIBS", camlib_dir.absolutePath().toStdString());
        }
    }
#else
#warning "No libgphoto2 environment variables specified for this operating system. If you want to deploy the application stand-alone you probably have to set them."
#endif
    QApplication app(argc, argv);
    app.setOrganizationName("Aalto");
    app.setOrganizationDomain("aalto.fi");
    app.setApplicationName("CaptureQuick");

    qmlRegisterUncreatableType<Capture>("CaptureQuick", 0, 1, "Capture", "Capture can't be instantiated from QML");

    // Makes the Camera class's Enum available to QML
    qmlRegisterUncreatableType<Camera>("CaptureQuick", 0, 1, "Camera", "Cameras can't be instantiated from QML");
    qmlRegisterUncreatableType<Image>("CaptureQuick", 0, 1, "CQImage", "Images can't be instantiated from QML");
    // Register meta types to make them usable in signal / slot connections
    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<QSharedPointer<Image>>("QSharedPointer<Image>");
    qRegisterMetaType<Camera::CameraState>("CameraState");

    QQmlApplicationEngine engine;

    Capture capture;

    // will be freed by Qt
    LiveImageProvider *liveImgProvider = LiveImageProvider::getInstance();
    engine.addImageProvider("live", liveImgProvider);

    engine.rootContext()->setContextProperty("capture", &capture);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    capture.connect();

    return app.exec();
}
