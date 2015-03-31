#include "image.h"

#include <fstream>

Image::Image() {}

Image::Image(const Image &other) :
    m_buffer(other.m_buffer),
    m_qimage(other.m_qimage) {}

Image::Image(const std::vector<char> &buffer) :
    m_buffer(buffer)
{
    m_qimage = QImage::fromData(reinterpret_cast<const uchar*>(buffer.data()), buffer.size());
}

Image::~Image() {}

const QImage& Image::toQImage() const {
    return m_qimage;
}

char const* Image::data() const {
    return m_buffer.data();
}

std::size_t Image::size() const {
    return m_buffer.size();
}

void Image::save(const std::string& fileName) const {
    // TODO: error handling
    std::ofstream out(fileName, std::ofstream::binary);
    out.write(m_buffer.data(), m_buffer.size());
    out.close();
}
