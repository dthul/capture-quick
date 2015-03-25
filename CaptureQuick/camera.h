#pragma once

#include <atomic>

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>

#include "cameracontroller.h"
#include "cameraeventlistener.h"
#include "gputil.h"

class Camera : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

    Q_PROPERTY(QString previewUrl READ previewUrl NOTIFY previewUrlChanged)

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

    QString previewUrl() const;
    const QImage& latestPreview() const;

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

    void previewUrlChanged(QString newPreviewUrl);

public slots:
private slots:
    void c_setName(const QString& name);
    void c_setPreviewImage(const QImage preview);
    void c_setApertureChoices(const QList<QString>& newApertureChoices);
    void c_setShutterChoices(const QList<QString>& newShutterChoices);
    void c_setIsoChoices(const QList<QString>& newIsoChoices);
    void c_setApertureIndex(const int aperture);
    void c_setShutterIndex(const int shutter);
    void c_setIsoIndex(const int iso);
private:
    void readConfig();

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

    QImage m_latest_preview;
    QDateTime m_latest_preview_time;

    static std::atomic_uint s_id;
    enum CameraState{
        CAMERA_SHUTDOWN,
        CAMERA_INIT,
        CAMERA_PREVIEW,
        CAMERA_NONE
    };
    volatile CameraState m_state;
};
