#include "liveimageprovider.h"

#include <iostream>

#include <QRegExp>

LiveImageProvider* LiveImageProvider::liveImageProvider = nullptr;

LiveImageProvider::LiveImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image),
    m_default_image(QImage(":/testchart.png"))
{

}

LiveImageProvider::~LiveImageProvider()
{

}

LiveImageProvider* LiveImageProvider::getInstance() {
    if (!liveImageProvider)
        liveImageProvider = new LiveImageProvider();
    return liveImageProvider;
}

void LiveImageProvider::registerImage(const class Image *const image) {
    m_imageMap.insert(image->id(), image);
}

void LiveImageProvider::unregisterImage(const class Image *const image) {
    m_imageMap.remove(image->id());
}

QString LiveImageProvider::urlFor(const class Image *const image) const {
    return QString::number(image->id());
}

QImage LiveImageProvider::requestImage(const QString &url, QSize *size, const QSize &/*requestedSize*/) {
    const uint64_t imageId = url.toULongLong();
    ImageMap::const_iterator it = m_imageMap.find(imageId);
    if (it != m_imageMap.end()) {
        QImage image = it.value()->toQImage();
        *size = image.size();
        return image;
    }
    *size = m_default_image.size();
    return m_default_image;
}
