#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "camera.h"
#include "capture.h"
#include "liveimageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    LiveImageProvider *liveImgProvider = new LiveImageProvider();
    engine.addImageProvider("live", liveImgProvider);

    Capture capture(&engine);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
