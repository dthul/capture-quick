#pragma once

#include <QObject>

class ImageModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit ImageModel(QObject *parent = 0);
    ImageModel(const QString &url, QObject *parent = 0);
    ~ImageModel();

    QString url() const;
    void setUrl(const QString &url);

signals:
    void urlChanged(QString newUrl);
public slots:
private:
    QString m_url;
};
