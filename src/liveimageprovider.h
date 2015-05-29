#pragma once

#include <QImage>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QQuickImageProvider>

#include "image.h"

class LiveImageProvider : public QQuickImageProvider
{
    typedef QMap<uint64_t, const class Image *> ImageMap;
private:
    LiveImageProvider();
public:
    static LiveImageProvider* getInstance();
    ~LiveImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void registerImage(const class Image *const image);
    void unregisterImage(const class Image *const image);
    QString urlFor(const class Image *const image) const;
private:
    QImage m_default_image;
    static LiveImageProvider* liveImageProvider;
    ImageMap m_imageMap;
    mutable QMutex m_mutex;
    static QMutex s_mutex;
};
