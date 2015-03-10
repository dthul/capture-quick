#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "imagemodel.h"
#include "liveimageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    LiveImageProvider *liveImgProvider = new LiveImageProvider();
    engine.addImageProvider("live", liveImgProvider);

    QList<QObject*> dataList;
    dataList.append(new ImageModel("url1"));
    //QStringList dataList;
    //dataList.append("Item 1");
    engine.rootContext()->setContextProperty("imageGridModel", QVariant::fromValue(dataList));
    //engine.rootContext()->setContextProperty("liveImageProvider", (QObject*)liveImgProvider);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
