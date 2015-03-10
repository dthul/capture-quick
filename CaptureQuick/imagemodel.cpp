#include "imagemodel.h"

ImageModel::ImageModel(QObject *parent) : QObject(parent)
{

}

ImageModel::ImageModel(const QString &url, QObject *parent) : QObject(parent), m_url(url) {}

ImageModel::~ImageModel()
{

}

void ImageModel::setUrl(const QString& url) {
    if (url != m_url) {
        m_url = url;
        emit urlChanged(url);
    }
}

QString ImageModel::url() const {
    return m_url;
}
