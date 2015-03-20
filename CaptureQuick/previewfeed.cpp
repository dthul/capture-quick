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
            std::cout << "Waiting for preview..." << std::endl;
            jpeg = m_camera->preview();
        } catch (gp::Exception& ex) {
            std::cout << "cam " /* TODO << index*/ << ": " << ex.what() << std::endl;
            continue;
        }
        // Convert the buffer containing jpeg data to a QImage
        QImage previewImage(QImage::fromData(reinterpret_cast<const uchar*>(jpeg.data()), jpeg.size()));
        std::cout << "emitting new preview image" << std::endl;
        emit newPreviewImage(previewImage);
    }
    emit stopped();
}

PreviewFeed::~PreviewFeed()
{

}
