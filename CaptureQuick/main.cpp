#ifdef MACOS
#include <cstdlib>
#endif

#include <QApplication>
#include <QFileInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "camera.h"
#include "capture.h"
#include "image.h"
#include "liveimageprovider.h"

int main(int argc, char *argv[])
{
#ifdef MACOS
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
