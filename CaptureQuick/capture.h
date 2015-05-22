#pragma once

#include <memory>

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

    Q_PROPERTY(int numCaptured READ numCaptured NOTIFY numCapturedChanged)
    Q_PROPERTY(QString captureRoot READ captureRoot WRITE setCaptureRoot NOTIFY captureRootChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(bool saveJpeg READ saveJpeg WRITE setSaveJpeg NOTIFY saveJpegChanged)
    Q_PROPERTY(bool saveRaw READ saveRaw WRITE setSaveRaw NOTIFY saveRawChanged)
    Q_PROPERTY(QQmlListProperty<Camera> allCameras READ allCameras NOTIFY allCamerasChanged)
    Q_PROPERTY(QQmlListProperty<Camera> uiCameras READ uiCameras NOTIFY uiCamerasChanged)
    Q_PROPERTY(QList<int> uiCameraRotations READ uiCameraRotations NOTIFY uiCameraRotationsChanged)
    Q_PROPERTY(int numRows READ numRows NOTIFY numRowsChanged)
    Q_PROPERTY(int numCols READ numCols NOTIFY numColsChanged)
    Q_PROPERTY(bool allConfigured READ allConfigured NOTIFY allConfiguredChanged)
public:
    explicit Capture(QObject *parent = 0);
    ~Capture();

    int numCaptured() const;
    QString captureRoot() const;
    void setCaptureRoot(const QString& newCaptureRoot);
    bool autoSave() const;
    bool saveJpeg() const;
    bool saveRaw() const;
    void setAutoSave(bool autoSave);
    void setSaveJpeg(bool saveJpeg);
    void setSaveRaw(bool saveRaw);
    QQmlListProperty<Camera> allCameras();
    QQmlListProperty<Camera> uiCameras();
    QList<int> uiCameraRotations();
    int numRows() const;
    int numCols() const;
    bool allConfigured() const;
    static int countUiCameras(QQmlListProperty<Camera> *list);
    static Camera* atUiCameras(QQmlListProperty<Camera> *list, int index);
signals:
    void numCapturedChanged(const int numCaptured);
    void captureRootChanged(const QString& captureRoot);
    void autoSaveChanged(bool autoSave);
    void saveJpegChanged(bool saveJpeg);
    void saveRawChanged(bool saveRaw);
    void numColsChanged(const int numCols);
    void numRowsChanged(const int numRows);
    void alert(QString message);
    void allConfiguredChanged(bool configured);
    void allCamerasChanged();
    void uiCamerasChanged();
    void uiCameraRotationsChanged();
public slots:
    void newCapture();
    void saveCaptureToDisk();
    void focusAll();
    void triggerAll();
    void loadCameraArrangementFromFile(const QString& fileName);
    void writeCameraArrangementToFile(const QString& fileName);
    void resetCameraArrangement();
    void connect();
    void disconnect();
private slots:
    void newImageCaptured(Image* image);
    void cameraNameChanged(const QString& name);
private:
    void recalculateGridSize();
    QString serializeCameraArrangement();
    void loadCameraArrangement(QString arrangement);
    void loadCameraArrangementFromSettings();
    Camera* findCameraByName(const QString& name);
    QList<Camera*> m_cameras; // A list of all connected cameras. Does not contain null pointers
    QList<Camera*> m_ui_cameras; // The arrangement of cameras in the UI. May contain null pointers
    QList<int> m_ui_camera_rotations;
    gp::Context gpcontext;
    std::vector<std::shared_ptr<gp::Camera>> m_gp_cameras;
    int m_num_captured;
    TriggerBox *m_triggerBox;
    QThread *m_triggerBoxThread;
    int m_num_rows;
    int m_num_cols;
    bool m_all_camera_names_known;
    bool m_connected;
};
