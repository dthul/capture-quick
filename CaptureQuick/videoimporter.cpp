#include "videoimporter.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>

#include <QMutexLocker>
#include <QRunnable>

#include "camera.h"
#include "persist.h"

QString FileInfo::filePath() const {
    return folder + "/" + fileName;
}

ImportInfo::ImportInfo(Camera *const camera, QObject *parent) :
    QObject(parent),
    m_camera(camera) {}

ImportInfo::~ImportInfo() {
    for (auto fileInfo : m_file_infos)
        delete fileInfo;
}

QQmlListProperty<FileInfo> ImportInfo::fileInfos() {
    return QQmlListProperty<FileInfo>(this, m_file_infos);
}

QList<FileInfo*> ImportInfo::fileInfosAsList() {
    return m_file_infos;
}

Camera* ImportInfo::camera() {
    return m_camera;
}

VideoImporter::VideoImporter(Capture *const capture, QObject *parent) :
    QObject(parent),
    m_capture(capture),
    m_num_videos(1),
    m_min_num_videos(0),
    m_import_running(false) {}

VideoImporter::~VideoImporter() {}

uint16_t VideoImporter::numVideos() const {
    return m_num_videos;
}

void VideoImporter::setNumVideos(const uint16_t new_num) {
    if (new_num == m_num_videos)
        return;
    m_num_videos = new_num;
    emit numVideosChanged(m_num_videos);
    // TODO: don't do a complete refresh when this number changes
    // (especially: don't re-read the cameras' filesystems again since it
    // takes time).
    refresh();
}

bool VideoImporter::deleteFromCamera() const {
    QSettings settings;
    return settings.value("videos/delete_from_camera", false).toBool();
}

void VideoImporter::setDeleteFromCamera(const bool newDeleteFromCamera) {
    QSettings settings;
    settings.setValue("videos/delete_from_camera", newDeleteFromCamera);
    emit deleteFromCameraChanged(newDeleteFromCamera);
}

QString VideoImporter::videoRoot() const {
    QSettings settings;
    const QString defaultVideoLocation =
            QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    return settings.value("videos/root_path", defaultVideoLocation).toString();
}

void VideoImporter::setVideoRoot(const QString& newVideoRoot) {
    QSettings settings;
    settings.setValue("videos/root_path", newVideoRoot);
    emit videoRootChanged(newVideoRoot);
}

uint16_t VideoImporter::minNumVideos() const {
    return m_min_num_videos;
}

int VideoImporter::countImportInfos(QQmlListProperty<ImportInfo> *list) {
    VideoImporter *video_importer = qobject_cast<VideoImporter*>(list->object);
    if (video_importer) {
        return video_importer->m_import_infos.length();
    }
    return 0;
}

ImportInfo* VideoImporter::atImportInfos(QQmlListProperty<ImportInfo> *list, int index) {
    VideoImporter *video_importer = qobject_cast<VideoImporter*>(list->object);
    if (video_importer && index >= 0 && index < video_importer->m_import_infos.size()) {
        return video_importer->m_import_infos.at(index).data();
    }
    return nullptr;
}

QQmlListProperty<ImportInfo> VideoImporter::importInfos() {
    return QQmlListProperty<ImportInfo>(this, 0, &VideoImporter::countImportInfos, &VideoImporter::atImportInfos);
}

void VideoImporter::refresh() {
    {
        QMutexLocker locker(&m_mutex);
        if (m_import_running)
            return;
    }
    QList<Camera*> allCameras = m_capture->allCamerasAsList();
    // Don't destroy the ImportInfos in m_import_infos yet, but after the emit
    QList<QSharedPointer<ImportInfo>> tmp = m_import_infos;
    m_import_infos.clear();
    m_min_num_videos = std::numeric_limits<uint16_t>::max();
    auto filterVideos = [](ImportInfo *const importInfo, const gp::Camera::FileInfo& fileInfo) -> void {
        QString const fileName = QString::fromStdString(fileInfo.name);
        if (fileName.toLower().endsWith(".mov")) {
            QString const folder = QString::fromStdString(fileInfo.folder);
            // ownership will be taken by importInfo
            FileInfo *importFileInfo = new FileInfo();
            importFileInfo->folder = folder;
            importFileInfo->fileName = fileName;
            importFileInfo->index = fileInfo.time;
            importInfo->m_file_infos.append(importFileInfo);
        }
    };
    // TODO: make this loop parallel since reading the camera's file system takes some time
    for (auto camera : allCameras) {
        QSharedPointer<ImportInfo> info(new ImportInfo(camera));

        // find all video files on the camera and store their paths in "info"
        camera->gp_camera()->for_each_file(std::bind(filterVideos, info.data(), std::placeholders::_1));

        // update the minimum number of videos on each camera
        if (info->m_file_infos.length() < m_min_num_videos)
            m_min_num_videos = info->m_file_infos.length();

        // sort the video files by their index (which is initialized to their creation time) in descending order
        auto fileInfoComparator = [](FileInfo* info1, FileInfo* info2) -> bool { return info1->index > info2->index; };
        std::sort(info->m_file_infos.begin(), info->m_file_infos.end(), fileInfoComparator);

        // shorten the QList to the number of requested videos
        if (info->m_file_infos.length() > m_num_videos)
            info->m_file_infos.erase(info->m_file_infos.begin() + m_num_videos, info->m_file_infos.end());

        m_import_infos.append(info);
    }
    if (allCameras.length() == 0)
        m_min_num_videos = 0;
    emit importInfosChanged();
    emit minNumVideosChanged(m_min_num_videos);
    tmp.clear();
}

class SaveImportInfoTask : public QRunnable
{
public:
    SaveImportInfoTask(VideoImporter *const videoImporter, QSharedPointer<ImportInfo> importInfo) :
        videoImporter(videoImporter),
        importInfo(importInfo) {}
private:
    struct SaveDoneNotifier {
        SaveDoneNotifier(VideoImporter *videoImporter) : videoImporter(videoImporter) {}
        ~SaveDoneNotifier() { videoImporter->saveDone(); }
        VideoImporter *videoImporter;
    };

    void run() {
        // Since C++ does not support finally blocks to execute
        // videoImporter->saveDone even in case of an exception
        // we use the RAII principle to make sure that it gets called.
        SaveDoneNotifier notifier(videoImporter);
        Persist::getInstance()->save(importInfo.data());
    }
    VideoImporter *videoImporter;
    QSharedPointer<ImportInfo> importInfo;
};

void VideoImporter::save() {
    if (m_import_running)
        return;
    if (QThreadPool::globalInstance()->maxThreadCount() < m_import_infos.length())
        QThreadPool::globalInstance()->setMaxThreadCount(m_import_infos.length());
    {
        QMutexLocker locker(&m_mutex);
        m_num_saves_missing = m_import_infos.length();
        if (m_num_saves_missing == 0)
            return;
        m_import_running = true;
        for (auto importInfo : m_import_infos)
            QThreadPool::globalInstance()->start(new SaveImportInfoTask(this, importInfo));
    }
    emit importRunningChanged(m_import_running);
    // QThreadPool::globalInstance()->waitForDone(8 * 1000);
}

void VideoImporter::saveDone() {
    {
        QMutexLocker locker(&m_mutex);
        --m_num_saves_missing;
        if (m_num_saves_missing == 0)
            m_import_running = false;
    }
    emit importRunningChanged(m_import_running);
}

bool VideoImporter::importRunning() const {
    QMutexLocker locker(&m_mutex); // Is this mutex lock needed to establish memory "consistency"?
    return m_import_running;
}
