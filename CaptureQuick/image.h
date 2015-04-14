#pragma once

#include <string>
#include <vector>

#include <QImage>

/**
 * This class stores an image buffer that has been read
 * from the camera.
 * The data is offered as a QImage for the UI and as a
 * raw buffer for saving or extracting the Exif data.
 */
class Image
{
public:
    Image();
    Image(const Image &other);
    Image(const std::vector<char>& buffer);
    ~Image();

    const QImage& toQImage() const;
    char const* data() const;
    std::size_t size() const;
    void save(const std::string& fileName);
private:
    std::vector<char> m_buffer;
    QImage m_qimage;
    std::string m_file_path;
};

Q_DECLARE_METATYPE(Image)
