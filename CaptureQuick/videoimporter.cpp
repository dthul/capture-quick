#include "videoimporter.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>

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
    m_min_num_videos(0) {}

VideoImporter::~VideoImporter()
{

}

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
    emit importInfosChanged();
    emit minNumVideosChanged(m_min_num_videos);
    tmp.clear();
}

void VideoImporter::save() {
    for (auto importInfo : m_import_infos)
        Persist::getInstance()->save(importInfo.data());
}
