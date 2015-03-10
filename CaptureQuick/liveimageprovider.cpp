#include "liveimageprovider.h"

#include <iostream>

LiveImageProvider::LiveImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{

}

LiveImageProvider::~LiveImageProvider()
{

}

QImage LiveImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    QImage img(":/testchart.png");
    img = img.scaled(QSize(100, 100));
    size = new QSize(100, 100);
    if (img.isNull())
        std::cout << "whoopsie" << std::endl;
    std::cout << id.toStdString() << std::endl;
    return img;
}
