#pragma once

#include <QDateTime>
#include <QDir>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlListProperty>

#include "camera.h"
#include "gputil.h"
#include "liveimageprovider.h"
#include "triggerbox.h"

class Capture : public QObject
{
    Q_OBJECT
private:
    Q_PROPERTY(int numCaptured READ numCaptured NOTIFY numCapturedChanged)
    Q_PROPERTY(QString captureRoot READ captureRoot WRITE setCaptureRoot NOTIFY captureRootChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(QQmlListProperty<Camera> allCameras READ allCameras NOTIFY allCamerasChanged)
    Q_PROPERTY(QQmlListProperty<Camera> uiCameras READ uiCameras NOTIFY uiCamerasChanged)
    Q_PROPERTY(int numRows READ numRows NOTIFY numRowsChanged)
    Q_PROPERTY(int numCols READ numCols NOTIFY numColsChanged)
    Q_PROPERTY(bool allConfigured READ allConfigured NOTIFY allConfiguredChanged)
public:
    Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent = 0);
    ~Capture();

    int numCaptured() const;
    QString captureRoot() const;
    void setCaptureRoot(const QString& newCaptureRoot);
    bool autoSave() const;
    void setAutoSave(bool autoSave);
    QQmlListProperty<Camera> allCameras();
    QQmlListProperty<Camera> uiCameras();
    int numRows() const;
    int numCols() const;
    bool allConfigured() const;
    static int countUiCameras(QQmlListProperty<Camera> *property);
    static Camera* atUiCameras(QQmlListProperty<Camera> *property, int index);
signals:
    void numCapturedChanged(const int numCaptured);
    void captureRootChanged(const QString& captureRoot);
    void autoSaveChanged(bool autoSave);
    void allCamerasChanged(QQmlListProperty<Camera> allCameras);
    void uiCamerasChanged(QQmlListProperty<Camera> allCameras);
    void numColsChanged(const int numCols);
    void numRowsChanged(const int numRows);
    void alert(QString message);
    void allConfiguredChanged(bool configured);
public slots:
    void newCapture();
    void saveCaptureToDisk();
    void focusAll();
    void triggerAll();
    void loadCameraArrangementFromFile(const QString& fileName);
    void writeCameraArrangementToFile(const QString& fileName);
    void resetCameraArrangement();
private slots:
    void newImageCaptured();
    void cameraNameChanged(const QString& name);
private:
    void recalculateGridSize();
    QString serializeCameraArrangement();
    void loadCameraArrangement(QString arrangement);
    void loadCameraArrangementFromSettings();
    Camera* findCameraByName(const QString& name);
    QQmlApplicationEngine* const m_qml_engine;
    QList<Camera*> m_cameras; // A list of all connected cameras. Does not contain null pointers
    QList<Camera*> m_ui_cameras; // The arrangement of cameras in the UI. May contain null pointers
    gp::Context gpcontext;
    std::vector<gp::Camera> m_gp_cameras;
    int m_num_captured;
    QDateTime m_capture_time;
    QString m_capture_root;
    bool m_auto_save;
    TriggerBox *m_triggerBox;
    QThread *m_triggerBoxThread;
    int m_num_rows;
    int m_num_cols;
    bool m_all_camera_names_known;
};
