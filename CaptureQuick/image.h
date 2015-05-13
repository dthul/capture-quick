#pragma once

#include <string>
#include <vector>

#include <QImage>
#include <QObject>

/**
 * This class stores an image buffer that has been read
 * from the camera.
 * The data is offered as a QImage for the UI and as a
 * raw buffer for saving or extracting the Exif data.
 */
class Image : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int saved READ saved NOTIFY savedChanged)
    Q_PROPERTY(QString url READ url CONSTANT)
public:
    Image(QObject *parent = 0);
    Image(const QString& url, QObject *parent = 0);
    Image(const std::vector<char>& buffer, QObject *parent = 0);
    ~Image();

    const QImage& toQImage() const;
    char const* data() const;
    std::size_t size() const;
    void save(const std::string& fileName);
    static bool is_jpeg(std::vector<char>const& image_data);
    bool is_raw() const;
    bool saved() const;
    QString url() const;
    uint64_t id() const;
signals:
    void savedChanged(bool saved);
private:
    void registerMe();
    void unregisterMe();
    std::vector<char> m_buffer;
    const uint64_t m_id;
    QImage m_qimage;
    std::string m_file_path;
    bool m_raw;
};
