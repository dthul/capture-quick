#pragma once

#include <QMutex>
#include <QObject>
#include <QQmlListProperty>
#include <QSharedPointer>
#include <QStringList>

#include "capture.h"

class VideoImporter;

class FileInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath CONSTANT)
public:
    QString folder;
    QString fileName;
    int index;
    QString filePath() const;
};

class ImportInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<FileInfo> fileInfos READ fileInfos CONSTANT)
    Q_PROPERTY(Camera* camera READ camera CONSTANT)
public:
    ImportInfo(Camera *const camera, QObject *parent = 0);
    ~ImportInfo();
    QQmlListProperty<FileInfo> fileInfos();
    QList<FileInfo*> fileInfosAsList();
    Camera* camera();
signals:
private:
    QList<FileInfo*> m_file_infos;
    Camera* const m_camera;
    friend class VideoImporter;
};

class VideoImporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ImportInfo> importInfos READ importInfos NOTIFY importInfosChanged)
    Q_PROPERTY(uint16_t numVideos READ numVideos WRITE setNumVideos NOTIFY numVideosChanged)
    Q_PROPERTY(uint16_t minNumVideos READ minNumVideos NOTIFY minNumVideosChanged)
    Q_PROPERTY(bool importRunning READ importRunning NOTIFY importRunningChanged)
    Q_PROPERTY(QString videoRoot READ videoRoot WRITE setVideoRoot NOTIFY videoRootChanged)
    Q_PROPERTY(bool deleteFromCamera READ deleteFromCamera WRITE setDeleteFromCamera NOTIFY deleteFromCameraChanged)
public:
    explicit VideoImporter(Capture *const capture, QObject *parent = 0);
    ~VideoImporter();

    static int countImportInfos(QQmlListProperty<ImportInfo> *list);
    static ImportInfo* atImportInfos(QQmlListProperty<ImportInfo> *list, int index);
    QQmlListProperty<ImportInfo> importInfos();
    uint16_t numVideos() const;
    void setNumVideos(const uint16_t new_num);
    uint16_t minNumVideos() const;
    bool importRunning() const;
    QString videoRoot() const;
    void setVideoRoot(const QString& newVideoRoot);
    bool deleteFromCamera() const;
    void setDeleteFromCamera(const bool newDeleteFromCamera);
signals:
    void importInfosChanged();
    void numVideosChanged(const uint16_t new_num);
    void minNumVideosChanged(const uint16_t new_min_num);
    void importRunningChanged(const bool importRunning);
    void videoRootChanged(const QString& newVideoRoot);
    void deleteFromCameraChanged(const bool newDeleteFromCamera);
public slots:
    void refresh();
    void save();
private:
    void saveDone();
    Capture *const m_capture;
    QList<QSharedPointer<ImportInfo>> m_import_infos;
    uint16_t m_num_videos; // How many most recent videos should be downloaded?
    uint16_t m_min_num_videos; // How many videos does the camera with the fewest videos contain?
    bool m_import_running;
    uint16_t m_num_saves_missing;
    mutable QMutex m_mutex;
    friend class SaveImportInfoTask;
};
