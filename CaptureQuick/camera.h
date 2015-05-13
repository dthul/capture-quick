#pragma once

#include <atomic>

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QThread>

#include "cameracontroller.h"
#include "cameraeventlistener.h"
#include "image.h"
#include "gputil.h"

class Camera : public QObject
{
    Q_OBJECT
public:
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

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

    Q_PROPERTY(Image* preview READ preview NOTIFY previewChanged)
    Q_PROPERTY(Image* image READ image NOTIFY imageChanged)
    Q_PROPERTY(Image* rawImage READ rawImage NOTIFY rawImageChanged)

    // Define some properties so that they are accessible from QML
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
    Camera(gp::Camera* const gp_camera, QObject *parent = 0);
    ~Camera();

    QString name() const;
    QString aperture() const;
    QString shutter() const;
    QString iso() const;

    int apertureIndex() const;
    int shutterIndex() const;
    int isoIndex() const;

    void setApertureIndex(const int index);
    void setShutterIndex(const int index);
    void setIsoIndex(const int index);

    QStringList apertureChoices() const;
    QStringList shutterChoices() const;
    QStringList isoChoices() const;

    void startPreview();
    void stopPreview();

    /*
    QString previewUrl() const;
    const QImage& latestPreview() const;

    QString imageUrl() const;
    const QImage& latestImage() const;

    QString rawImageUrl() const;
    const QImage& latestRawImage() const;
    */
    Image* preview();
    Image* image();
    Image* rawImage();

    CameraState state() const;
    void setState(const CameraState state);

    const uint m_id;

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
    void trigger();
    void clearLatestImage();
    void saveImage(const QString& fileName);
    void saveRawImage(const QString& fileName);
    void readConfig();
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
    void c_previewStarted();
    void c_previewStopped();
    void c_resetDone();

private:
    // Will request a configuration read from the controller.
    void reset();

    gp::Camera* m_camera = nullptr;

    QStringList m_apertureChoices;
    QStringList m_shutterChoices;
    QStringList m_isoChoices;

    QString m_name;
    int m_aperture = -1;
    int m_shutter = -1;
    int m_iso = -1;
    QThread *m_controllerThread = nullptr;
    CameraController *m_controller = nullptr;
    QThread *m_eventListenerThread = nullptr;
    CameraEventListener *m_eventListener = nullptr;

    QSharedPointer<Image> m_latest_preview;

    QSharedPointer<Image> m_latest_image;

    QSharedPointer<Image> m_latest_raw_image;

    static std::atomic_uint s_id;
    CameraState m_state;
};
