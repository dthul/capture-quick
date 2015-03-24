#pragma once

#include <atomic>

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QString>
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
    Q_PROPERTY(QString aperture READ aperture WRITE setAperture NOTIFY apertureChanged)
    Q_PROPERTY(QString shutter READ shutter WRITE setShutter NOTIFY shutterChanged)
    Q_PROPERTY(QString iso READ iso WRITE setIso NOTIFY isoChanged)

    Q_PROPERTY(QList<QString> apertureChoices READ apertureChoices NOTIFY apertureChoicesChanged)
    Q_PROPERTY(QList<QString> shutterChoices READ shutterChoices NOTIFY shutterChoicesChanged)
    Q_PROPERTY(QList<QString> isoChoices READ isoChoices NOTIFY isoChoicesChanged)
public:
    explicit Camera(QObject *parent = 0);
    Camera(gp::Camera* const gp_camera, QObject *parent = 0);
    ~Camera();

    QString name() const;
    QString aperture() const;
    void setAperture(const QString &aperture);
    QString shutter() const;
    void setShutter(const QString &shutter);
    QString iso() const;
    void setIso(const QString &iso);

    QList<QString> apertureChoices() const;
    QList<QString> shutterChoices() const;
    QList<QString> isoChoices() const;

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

    void apertureChoicesChanged(const QList<QString>& newApertureChoices);
    void shutterChoicesChanged(const QList<QString>& newShutterChoices);
    void isoChoicesChanged(const QList<QString>& newIsoChoices);

    void previewUrlChanged(QString newPreviewUrl);

public slots:
private slots:
    void setName(const QString& name);
    void setPreviewImage(const QImage preview);
    void setApertureChoices(const QList<QString>& newApertureChoices);
    void setShutterChoices(const QList<QString>& newShutterChoices);
    void setIsoChoices(const QList<QString>& newIsoChoices);
    void setApertureIndex(const int aperture);
    void setShutterIndex(const int shutter);
    void setIsoIndex(const int iso);
private:
    void readConfig();

    gp::Camera* m_camera = nullptr;

    QList<QString> m_apertureChoices;
    QList<QString> m_shutterChoices;
    QList<QString> m_isoChoices;

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
