#pragma once

#include <atomic>

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QThread>

#include "cameraeventlistener.h"
#include "image.h"
#include "gputil.h"

class CameraController;

/**
 *  A camera abstraction. Allows to query and set camera information and caches
 *  the latest images.
 */
class Camera : public QObject
{
    Q_OBJECT
public:
    /**
     * Describes the different states a camera can be in.
     */
    enum CameraState {
        CAMERA_SHUTDOWN,
        CAMERA_INIT,
        CAMERA_PREVIEW,
        CAMERA_CAPTURE,
        CAMERA_TRANSITIONING,
        CAMERA_NONE
    };
private:
    Q_ENUMS(CameraState)

    // Define some properties so that they are accessible from QML
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

    Q_PROPERTY(Image* preview READ preview NOTIFY previewChanged)
    Q_PROPERTY(Image* image READ image NOTIFY imageChanged)
    Q_PROPERTY(Image* rawImage READ rawImage NOTIFY rawImageChanged)

    Q_PROPERTY(QString aperture READ aperture NOTIFY apertureChanged)
    Q_PROPERTY(QString shutter READ shutter NOTIFY shutterChanged)
    Q_PROPERTY(QString iso READ iso NOTIFY isoChanged)

    Q_PROPERTY(int apertureIndex READ apertureIndex WRITE setApertureIndex NOTIFY apertureIndexChanged)
    Q_PROPERTY(int shutterIndex READ shutterIndex WRITE setShutterIndex NOTIFY shutterIndexChanged)
    Q_PROPERTY(int isoIndex READ isoIndex WRITE setIsoIndex NOTIFY isoIndexChanged)

    Q_PROPERTY(QStringList apertureChoices READ apertureChoices NOTIFY apertureChoicesChanged)
    Q_PROPERTY(QStringList shutterChoices READ shutterChoices NOTIFY shutterChoicesChanged)
    Q_PROPERTY(QStringList isoChoices READ isoChoices NOTIFY isoChoicesChanged)

    Q_PROPERTY(CameraState state READ state WRITE setState NOTIFY stateChanged)
public:
    explicit Camera(QObject *parent = 0);
    Camera(std::shared_ptr<gp::Camera> const gp_camera, QObject *parent = 0);
    ~Camera();

    QString name() const; /**< Name of the artist stored on the camera. Used to identify cameras. */
    QString aperture() const; /**< Currently selected aperture. */
    QString shutter() const; /**< Currently selected shutter. */
    QString iso() const; /**< Currently selected iso. */

    /**
     * @return currently selected aperture index.
     * @see setApertureIndex()
     * @see apertureChoices()
     */
    int apertureIndex() const;
    /**
     * @return currently selected shutter index.
     * @see setShutterIndex()
     * @see shutterChoices()
     */
    int shutterIndex() const;
    /**
     * @return currently selected iso index.
     * @see setIsoIndex()
     * @see isoChoices()
     */
    int isoIndex() const;

    /**
     * Asynchronously tries to change the camera's aperture.
     * On successful change the apertureChanged() signal will be emitted.
     * @param index index of the aperture to set
     * @see apertureChoices()
     */
    void setApertureIndex(const int index);
    /**
     * Asynchronously tries to change the camera's shutter speed.
     * On successful change the shutterChanged() signal will be emitted.
     * @param index index of the shutter speed to set
     * @see shutterChoices()
     */
    void setShutterIndex(const int index);
    /**
     * Asynchronously tries to change the camera's ISO.
     * On successful change the isoChanged() signal will be emitted.
     * @param index index of the ISO value to set
     * @see isoChoices()
     */
    void setIsoIndex(const int index);

    /**
     * @return a list of the camera's possible aperture choices
     */
    QStringList apertureChoices() const;
    /**
     * @return a list of the camera's possible shutter choices
     */
    QStringList shutterChoices() const;
    /**
     * @return a list of the camera's possible ISO choices
     */
    QStringList isoChoices() const;

    /**
     * Asynchronously switches the camera to the preview state.
     * On successful state change an appropriate stateChanged() signal will be
     * emitted.
     * For each captured preview image the newPreviewImage() signal will be
     * emitted.
     * No images can be captured while the camera is in preview mode.
     */
    void startPreview();
    /**
     * Asynchronously switches the camera to the capture state.
     * On successful state change an appropriate stateChanged() signal will be
     * emitted.
     * As soon as the camera is in the capture state you will be able to capture
     * images.
     */
    void stopPreview();

    /**
     * Caches the latest preview image.
     * This pointer will be valid at least until after the next previewChanged()
     * signal was emitted. Primarily intended to be called by QML.
     * @return the latest preview image or an empty image
     * @see Image::empty()
     */
    Image* preview();
    /**
     * Caches the latest captured image.
     * This pointer will be valid at least until after the next imageChanged()
     * signal was emitted. Primarily intended to be called by QML.
     * @return the latest captured image or an empty image
     * @see Image::empty()
     */
    Image* image();
    /**
     * Caches the latest captured RAW image.
     * This pointer will be valid at least until after the next
     * rawImageChanged() signal was emitted. Primarily intended to be called by
     * QML.
     * @return the latest captured RAW image or an empty image
     * @see Image::empty()
     */
    Image* rawImage();

    CameraState state() const; /**< the camera's current state */
    /**
     * Asynchronously tries to change the camera's state.
     * The stateChanged() signal will keep you informed about the camera's
     * current state.
     * @param state one of CameraState::CAMERA_PREVIEW or CameraState::CAMERA_CAPTURE
     */
    void setState(const CameraState state);

    /**
     * Exposes the underlying libgphoto wrapper camera object.
     * @return a shared pointer to the underlying libgphoto wrapper camera object
     */
    std::shared_ptr<gp::Camera> gp_camera();

signals:
    void nameChanged(const QString& newName);

    void apertureChanged(const QString& newAperture);
    void shutterChanged(const QString& newShutter);
    void isoChanged(const QString& newIso);

    void apertureIndexChanged(const int newAperture);
    void shutterIndexChanged(const int newIndex);
    void isoIndexChanged(const int newIndex);

    void apertureChoicesChanged(const QStringList& newApertureChoices);
    void shutterChoicesChanged(const QStringList& newShutterChoices);
    void isoChoicesChanged(const QStringList& newIsoChoices);

    void previewChanged(Image* newPreview);
    void imageChanged(Image* newImage);
    void rawImageChanged(Image* newRawImage);

    void stateChanged(const CameraState state);

public slots:
    /**
     * Will try to trigger this camera via USB.
     * Only works when the camera is in the capture state.
     */
    void trigger();
    void clearLatestImage();
    /**
     * Initiates an asynchronous read of the camera's current configuration.
     * Only needs to be called once after the camera object has been created.
     * Appropriate signals will be emitted once the configuration has been read.
     */
    void readConfig();
    /**
     * Asynchronously activates or deactivates the viewfinder. This can be used
     * to flip the mirror. While the camera is in preview mode this setting will
     * not have a lasting effect since the preview capturing will force the
     * viewfinder into a specific position.
     */
    void setViewfinder(bool on);
private slots:
    // These slots are invoked from the controller after
    // a configuration has been requested by this class
    // and answered by the (hardware) camera.
    void c_setName(const QString& name);
    void c_setPreviewImage(QSharedPointer<Image> preview);
    void c_setImage(QSharedPointer<Image> image);
    void c_setApertureChoices(const QList<QString>& newApertureChoices);
    void c_setShutterChoices(const QList<QString>& newShutterChoices);
    void c_setIsoChoices(const QList<QString>& newIsoChoices);
    void c_setApertureIndex(const int aperture);
    void c_setShutterIndex(const int shutter);
    void c_setIsoIndex(const int iso);
    void c_viewfinderChanged(const bool on);
    void c_previewStarted();
    void c_previewStopped();
    void c_resetDone();

private:
    /**
     * Asynchronously resets the camera by releasing an reacquiring it.
     */
    void reset();

    std::shared_ptr<gp::Camera> m_camera = nullptr;

    QStringList m_apertureChoices;
    QStringList m_shutterChoices;
    QStringList m_isoChoices;

    QString m_name; /**< caches the name */
    int m_aperture = -1; /**< caches the current aperture index */
    int m_shutter = -1; /**< caches the current shutter speed index */
    int m_iso = -1; /**< caches the current ISO index */
    QThread *m_controllerThread = nullptr;
    CameraController *m_controller = nullptr;
    QThread *m_eventListenerThread = nullptr;
    CameraEventListener *m_eventListener = nullptr;

    QSharedPointer<Image> m_latest_preview;

    QSharedPointer<Image> m_latest_image;

    QSharedPointer<Image> m_latest_raw_image;

    CameraState m_state;
};
