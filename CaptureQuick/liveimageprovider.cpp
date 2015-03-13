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
    // Ignore the requested size and just returned the image as-is
    *size = img.size();
    //if (img.isNull())
    //    std::cout << "whoopsie" << std::endl;
    //std::cout << id.toStdString() << std::endl;
    return img;
}
