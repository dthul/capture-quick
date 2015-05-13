#include "image.h"

#include <fstream>
#include <iostream>

#include "liveimageprovider.h"
#include "util.h"

Image::Image(QObject *parent) :
    QObject(parent),
    m_id(util::getId()) {
    registerMe();
}

Image::Image(const QString& url, QObject *parent) :
    QObject(parent),
    m_id(util::getId()),
    m_qimage(url) {
    registerMe();
}

Image::Image(const std::vector<char> &buffer, QObject *parent) :
    QObject(parent),
    m_buffer(buffer),
    m_id(util::getId())
{
    if (is_jpeg(buffer)) {
        m_qimage = QImage::fromData(reinterpret_cast<const uchar*>(buffer.data()), buffer.size());
        m_raw = false;
    }
    else {
        m_qimage = QImage::QImage(":/raw.png");
        m_raw = true;
    }
    registerMe();
}

Image::~Image() {
    unregisterMe();
}

void Image::registerMe() {
    LiveImageProvider::getInstance()->registerImage(this);
}

void Image::unregisterMe() {
    LiveImageProvider::getInstance()->unregisterImage(this);
}

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

bool Image::saved() const {
    return !m_file_path.empty();
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
    emit savedChanged(saved());
}

uint64_t Image::id() const {
    return m_id;
}

QString Image::url() const {
    return LiveImageProvider::getInstance()->urlFor(this);
}
