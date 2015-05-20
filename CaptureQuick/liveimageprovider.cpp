#include "liveimageprovider.h"

#include <iostream>

#include <QMutexLocker>
#include <QRegExp>

LiveImageProvider* LiveImageProvider::liveImageProvider = nullptr;
QMutex LiveImageProvider::s_mutex;

LiveImageProvider::LiveImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image),
    m_default_image(QImage(":/testchart.png"))
{

}

LiveImageProvider::~LiveImageProvider()
{

}

LiveImageProvider* LiveImageProvider::getInstance() {
    QMutexLocker locker(&s_mutex);
    if (!liveImageProvider)
        liveImageProvider = new LiveImageProvider();
    return liveImageProvider;
}

void LiveImageProvider::registerImage(const class Image *const image) {
    QMutexLocker locker(&m_mutex);
    m_imageMap.insert(image->id(), image);
}

void LiveImageProvider::unregisterImage(const class Image *const image) {
    QMutexLocker locker(&m_mutex);
    m_imageMap.remove(image->id());
}

QString LiveImageProvider::urlFor(const class Image *const image) const {
    QMutexLocker locker(&m_mutex);
    return QString::number(image->id());
}

QImage LiveImageProvider::requestImage(const QString &url, QSize *size, const QSize &/*requestedSize*/) {
    QMutexLocker locker(&m_mutex);
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
