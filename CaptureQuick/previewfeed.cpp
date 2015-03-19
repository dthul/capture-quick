#include "previewfeed.h"

#include <iostream>

PreviewFeed::PreviewFeed(gp::Camera* const camera, QObject *parent) :
    QObject(parent),
    m_camera(camera),
    m_stopped(false)
{

}

void PreviewFeed::stop() {
    m_stopped = true;
}

void PreviewFeed::start() {
    JpegBuffer jpeg;
    while (!m_stopped) {
        try {
            jpeg = m_camera->preview();
        } catch (gp::Exception& ex) {
            std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
            continue;
        }
        // Convert the buffer containing jpeg data to a QImage
        QImage previewImage(QImage::fromData(reinterpret_cast<const uchar*>(jpeg.data()), jpeg.size()));
        emit newPreviewImage(previewImage);
    }
    emit stopped();
}

PreviewFeed::~PreviewFeed()
{

}
