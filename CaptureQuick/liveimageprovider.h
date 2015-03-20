#pragma once

#include <QImage>
#include <QList>
#include <QQuickImageProvider>

#include "camera.h"

class LiveImageProvider : public QQuickImageProvider
{
public:
    LiveImageProvider(QList<Camera*>* cameras);
    ~LiveImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
    QList<Camera*>* m_cameras;
    QImage m_default_image;
};
