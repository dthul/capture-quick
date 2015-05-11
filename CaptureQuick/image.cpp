#include "image.h"

#include <fstream>
#include <iostream>

Image::Image() {}

Image::Image(const Image &other) :
    m_buffer(other.m_buffer),
    m_qimage(other.m_qimage) {}

Image::Image(const std::vector<char> &buffer) :
    m_buffer(buffer)
{
    if (is_jpeg(buffer)) {
        m_qimage = QImage::fromData(reinterpret_cast<const uchar*>(buffer.data()), buffer.size());
        m_raw = false;
    }
    else {
        m_qimage = QImage::QImage(":/raw.png");
        m_raw = true;
    }
}

Image::~Image() {}

bool Image::is_jpeg(std::vector<char>const & image_data) {
    unsigned char const * const unsigned_data = reinterpret_cast<const unsigned char*>(image_data.data());
    return image_data.size() > 1 && unsigned_data[0] == 0xFF && unsigned_data[1] == 0xD8;
}

bool Image::is_raw() const {
    return m_raw;
}

const QImage& Image::toQImage() const {
    return m_qimage;
}

char const* Image::data() const {
    return m_buffer.data();
}

std::size_t Image::size() const {
    return m_buffer.size();
}

void Image::save(const std::string& fileName) {
    // TODO: error handling
    if (m_buffer.size() == 0 || fileName == m_file_path)
        return;
    std::cout << "Saving as " << fileName << std::endl;
    std::ofstream out(fileName, std::ofstream::binary);
    out.write(m_buffer.data(), m_buffer.size());
    out.close();
    m_file_path = fileName;
}
