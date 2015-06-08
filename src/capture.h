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

/**
 *  This is the main class of the application that contains the "business" logic
 *  and interfaces with the UI via signals, slots and Q_PROPERTYs.
 *  Its main job is to keep track of the connected cameras and their arrangement
 *  in the UI and to actually capture images.
 */
class Capture : public QObject
{
    Q_OBJECT

    // Expose all necessary properties to QML
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
    Q_PROPERTY(Camera::CameraState commonState READ commonState NOTIFY commonStateChanged)
public:
    /**
     * Created a new Capture instance. You need to call connect() on the
     * resulting object to actually connect to the cameras.
     * @see connect()
     */
    explicit Capture(QObject *parent = 0);
    ~Capture();

    /**
     * TODO: remove this function
     * @return the number of (non-RAW) images that have been capture in the current capture round
     */
    int numCaptured() const;
    /**
     * Returns the directory that is stored in the application settings as the
     * root directory to store captured images in.
     * @return the root directory for captured images
     */
    QString captureRoot() const;
    /**
     * Changes the directory that is stored in the application settings as the
     * root directory to store captured images in.
     * @param newCaptureRoot the new root directory for capture images
     */
    void setCaptureRoot(const QString& newCaptureRoot);
    bool autoSave() const; /**< the current auto save setting */
    bool saveJpeg() const; /**< the current "save JPEG" setting */
    bool saveRaw() const;  /**< the current "save RAW" setting */
    void setAutoSave(bool autoSave); /**< changes the auto save setting */
    void setSaveJpeg(bool saveJpeg); /**< changes the "save JPEG" setting */
    void setSaveRaw(bool saveRaw); /**< changes the "save RAW" setting */
    /**
     * @return the connected cameras
     */
    QList<Camera*> allCamerasAsList();
    /**
     * Exposes all connected cameras to the UI as a QQmlListProperty.
     * @return the QQmlListProperty representing all connected cameras.
     */
    QQmlListProperty<Camera> allCameras();
    /**
     * Exposes all cameras to the UI that are part of the currently loaded
     * camera arrangement.
     * Might contain null pointers for empty spots in the arrangement.
     * @return the QQmlListProperty representing the current camera arrangement
     * @see uiCameraRotations()
     */
    QQmlListProperty<Camera> uiCameras();
    /**
     * Returns a list of rotations for the cameras that are part of the
     * currently loaded arrangement. Is as long as the list returned by
     * uiCameras().
     * @return the list of rotations
     * @see uiCameras()
     */
    QList<int> uiCameraRotations();
    /**
     * @return the number of rows necessary to display the current camera
     * arrangement
     */
    int numRows() const;
    /**
     * @return the number of columns necessary to display the current camera
     * arrangement
     */
    int numCols() const;
    /**
     * @return true if the configuration of all connected cameras has been read
     */
    bool allConfigured() const;
    /**
     * @return the common state of all connected cameras or CameraState::CAMERA_NONE if there is no common state
     */
    Camera::CameraState commonState() const;

    // The following functions are implementation details needed to construct
    // the QQmlListProperties
    static int countUiCameras(QQmlListProperty<Camera> *list);
    static Camera* atUiCameras(QQmlListProperty<Camera> *list, int index);
    static int countAllCameras(QQmlListProperty<Camera> *list);
    static Camera* atAllCameras(QQmlListProperty<Camera> *list, int index);
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
    void commonStateChanged();
    void allCamerasChanged();
    void uiCamerasChanged();
    void uiCameraRotationsChanged();
public slots:
    void newCapture();
    void saveCaptureToDisk();
    void halfPressShutterAll(const int milliseconds = 200);
    void pressShutterAll(const int milliseconds = 200);
    void loadCameraArrangementFromFile(const QString& fileName);
    void writeCameraArrangementToFile(const QString& fileName);
    void resetCameraArrangement();
    /**
     * Will try to connect to all the cameras that are currently connected to
     * the PC.
     * Does nothing if already connected. In that case you have to call
     * disconnect() first.
     * Will reset the camera arrangement if it contains cameras that were not
     * connected to.
     * @see disconnect()
     */
    void connect();
    /**
     * Will disconnect from all currently connected cameras and release them.
     * Does nothing if already disconnected.
     * @see connect()
     */
    void disconnect();
private slots:
    // These slots will receive signals from Camera objects
    void newImageCaptured(Image* image);
    void cameraNameChanged(const QString& name);
    void cameraStateChanged();
private:
    void recalculateGridSize();
    QString serializeCameraArrangement();
    void loadCameraArrangement(QString arrangement);
    void loadCameraArrangementFromSettings();
    Camera* findCameraByName(const QString& name);
    QList<Camera*> m_cameras; /**< A list of all connected cameras. Does not contain null pointers */
    QList<Camera*> m_ui_cameras; /**< The arrangement of cameras in the UI. May contain null pointers */
    QList<int> m_ui_camera_rotations;
    gp::Context gpcontext;
    std::vector<std::shared_ptr<gp::Camera>> m_gp_cameras;
    int m_num_captured; // TODO: remove me
    TriggerBox *m_triggerBox;
    QThread *m_triggerBoxThread;
    int m_num_rows;
    int m_num_cols;
    bool m_all_camera_names_known;
    bool m_connected;
};
