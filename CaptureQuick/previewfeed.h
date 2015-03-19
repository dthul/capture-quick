#ifndef PREVIEWFEED_H
#define PREVIEWFEED_H

#include <QImage>
#include <QObject>

#include "gputil.h"

class PreviewFeed : public QObject
{
    Q_OBJECT
    typedef std::vector<char> JpegBuffer;
public:
    PreviewFeed(gp::Camera* const camera, QObject *parent = 0);
    ~PreviewFeed();

signals:
    void newPreviewImage(QImage& preview);
    void stopped();
public slots:
    void start();
    void stop();
private:
    gp::Camera* m_camera;
    volatile bool m_stopped = false;
};

#endif // PREVIEWFEED_H
