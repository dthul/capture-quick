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
    QApplication app(argc, argv);

    // Makes the Camera class's Enum available to QML
    qmlRegisterUncreatableType<Camera>("CaptureQuick", 0, 1, "Camera", "Cameras can't be instantiated from QML");
    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<Image>("Image");

    QQmlApplicationEngine engine;

    Capture capture(&engine);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
