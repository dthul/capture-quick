#include "persist.h"

#include <QDateTime>
#include <QDir>
#include <QMutexLocker>
#include <QSettings>
#include <QStandardPaths>

#include "camera.h"

Persist* Persist::persist = nullptr;
QMutex Persist::s_mutex;

QString Persist::FileName::path() const {
    return dir + (dir.endsWith(QDir::separator()) ? QString("") : QDir::separator()) + name;
}

Persist::Persist() :
    m_time(QDateTime::currentMSecsSinceEpoch()) {}

Persist* Persist::getInstance() {
    QMutexLocker locker(&s_mutex);
    if (!persist)
        persist = new Persist();
    return persist;
}

void Persist::next() {
    QMutexLocker locker(&m_mutex);
    m_time = QDateTime::currentMSecsSinceEpoch();
}

void Persist::save(Image* const image) {
    QMutexLocker locker(&m_mutex);
    if (image->saved() || !image->camera() || image->empty())
        return;
    const FileName fileName = fileNameFor(image);
    // Make sure that the destination directory exists
    if (!QDir::root().mkpath(fileName.dir))
        // TODO: throw error instead
        return;
    const QDir destinationFolder(fileName.dir);
    // Use the current time as the filename
    if (destinationFolder.exists(fileName.name))
        // TODO: throw error instead
        return;
    image->save(fileName.path());
}

Persist::FileName Persist::fileNameFor(Image* const image) {
    QSettings settings;
    const QString defaultCaptureLocation =
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString captureRoot = QDir(settings.value("capture/root_path", defaultCaptureLocation).toString()).absolutePath();
    // There is no function in Qt to concatenate paths (wtf!) so we hackishly do it ourselves
    const bool addSeparator = !defaultCaptureLocation.endsWith(QDir::separator());
    FileName fileName;
    fileName.dir = captureRoot + (addSeparator ? QDir::separator() : QString("")) + QString::number(m_time) + QDir::separator();
    fileName.name = image->camera()->name() + (image->is_raw() ? ".cr2" : ".jpg");
    return fileName;
}
