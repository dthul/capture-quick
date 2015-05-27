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

bool Persist::checkFileName(const FileName& fileName) {
    // Make sure that the destination directory exists
    if (!QDir::root().mkpath(fileName.dir))
        return false;
    const QDir destinationFolder(fileName.dir);
    // Make sure that a file of this name does not already exist
    if (destinationFolder.exists(fileName.name))
        return false;
    return true;
}

void Persist::save(Image* const image) {
    QMutexLocker locker(&m_mutex);
    if (image->saved() || !image->camera() || image->empty())
        return;
    const FileName fileName = fileNameFor(image);
    if (!checkFileName(fileName)) {
        // TODO: throw error instead?
        return;
    }
    image->save(fileName.path());
}

void Persist::save(ImportInfo* const importInfo) {
    QMutexLocker locker(&m_mutex);
    const bool delete_from_cam = false; // TODO: make configurable
    const QList<FileName>  fileNames = fileNamesFor(importInfo);
    const QList<FileInfo*> fileInfos = importInfo->fileInfosAsList();
    Camera* const camera = importInfo->camera();
    for (uint16_t i = 0; i < fileNames.length(); ++i) {
        FileInfo *const fileInfo = fileInfos[i];
        const FileName& fileName = fileNames[i];
        if (!checkFileName(fileName))
            continue; // TODO: somehow communicate errors?
        camera->gp_camera()->save_file(
                    fileInfo->folder.toStdString(),
                    fileInfo->fileName.toStdString(),
                    fileName.path().toStdString(),
                    delete_from_cam);
    }
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

QList<Persist::FileName> Persist::fileNamesFor(ImportInfo* const importInfo) {
    QList<FileName> fileNames;
    QSettings settings;
    const QString defaultVideoLocation =
            QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    QString videoRoot = QDir(settings.value("videos/root_path", defaultVideoLocation).toString()).absolutePath();
    // There is no function in Qt to concatenate paths (wtf!) so we hackishly do it ourselves
    const bool addSeparator = !defaultVideoLocation.endsWith(QDir::separator());
    Camera *const camera = importInfo->camera();
    const int num_files = importInfo->fileInfosAsList().length();
    for (int i = 0; i < num_files; ++i) {
        FileName fileName;
        fileName.dir = videoRoot + (addSeparator ? QDir::separator() : QString("")) + QString::number(m_time) + QDir::separator();
        fileName.name = camera->name() + "_" + QString::number(i) + ".mov";
        fileNames.append(fileName);
    }
    return fileNames;
}
