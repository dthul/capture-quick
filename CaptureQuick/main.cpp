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
    const QFileInfo executable_path(QString::fromStdString(util::executable_path()));
    const std::string lib_dir = executable_path.absolutePath().toStdString();
    util::setenv("IOLIBS", lib_dir);
    util::setenv("CAMLIBS", lib_dir);
#ifdef __APPLE__
    system("killall PTPCamera");
#endif
    QApplication app(argc, argv);
    app.setOrganizationName("Aalto");
    app.setOrganizationDomain("aalto.fi");
    app.setApplicationName("CaptureQuick");

    qmlRegisterUncreatableType<Capture>("CaptureQuick", 0, 1, "Capture", "Capture can't be instantiated from QML");

    // Makes the Camera class's Enum available to QML
    qmlRegisterUncreatableType<Camera>("CaptureQuick", 0, 1, "Camera", "Cameras can't be instantiated from QML");
    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<Image>("Image");

    QQmlApplicationEngine engine;

    Capture capture(&engine);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
