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
    dataList.append(new ImageModel("url2"));
    dataList.append(new ImageModel("url3"));
    dataList.append(new ImageModel("url4"));
    dataList.append(new ImageModel("url5"));
    dataList.append(new ImageModel("url6"));
    dataList.append(new ImageModel("url7"));
    dataList.append(new ImageModel("url8"));
    dataList.append(new ImageModel("url9"));

    engine.rootContext()->setContextProperty("imageGridModel", QVariant::fromValue(dataList));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
