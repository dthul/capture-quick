#pragma once

#include <QDateTime>
#include <QDir>
#include <QObject>
#include <QQmlApplicationEngine>

#include "camera.h"
#include "gputil.h"
#include "liveimageprovider.h"

class Capture : public QObject
{
    Q_OBJECT
private:
    Q_PROPERTY(int numCaptured READ numCaptured NOTIFY numCapturedChanged)
    Q_PROPERTY(QString captureRoot READ captureRoot WRITE setCaptureRoot NOTIFY captureRootChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
public:
    Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent = 0);
    ~Capture();

    int numCaptured() const;
    QString captureRoot() const;
    void setCaptureRoot(const QString& newCaptureRoot);
    bool autoSave() const;
    void setAutoSave(bool autoSave);
signals:
    void numCapturedChanged(const int numCaptured);
    void captureRootChanged(const QString& captureRoot);
    void autoSaveChanged(bool autoSave);
public slots:
    void newCapture();
    void saveCaptureToDisk();
private slots:
    void newImageCaptured();
private:
    QQmlApplicationEngine* const m_qml_engine;
    QList<QObject*> m_cameras;
    gp::Context gpcontext;
    std::vector<gp::Camera> m_gp_cameras;
    int m_num_captured;
    QDateTime m_capture_time;
    QString m_capture_root;
    bool m_auto_save;
};
