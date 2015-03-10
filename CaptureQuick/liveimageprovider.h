#pragma once

#include <QImage>
#include <QQuickImageProvider>

class LiveImageProvider : public QQuickImageProvider
{
public:
    LiveImageProvider();
    ~LiveImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};
