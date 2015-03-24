#include "liveimageprovider.h"

#include <iostream>

#include <QRegExp>

const QRegExp url_regex("^([^/]+)/([^/]+)$");

LiveImageProvider::LiveImageProvider(QList<Camera*>* cameras) :
    QQuickImageProvider(QQmlImageProviderBase::Image),
    m_cameras(cameras),
    m_default_image(QImage(":/testchart.png"))
{

}

LiveImageProvider::~LiveImageProvider()
{

}

QImage LiveImageProvider::requestImage(const QString &url, QSize *size, const QSize &requestedSize) {
    *size = m_default_image.size();
    if(!url_regex.exactMatch(url))
        return m_default_image;
    const QString id_str = url_regex.capturedTexts()[1];
    bool ok;
    const uint id = id_str.toUInt(&ok);
    if (!ok)
        return m_default_image;
    for (Camera* cam : *m_cameras) {
        if (cam->m_id == id) {
            const QImage& preview = cam->latestPreview();
            *size = preview.size();
            return preview;
        }
    }
    return m_default_image;
}
