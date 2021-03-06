#pragma once

#include <memory>

#include <QFileInfo>
#include <QImage>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>

#include "camera.h"
#include "image.h"
#include "gputil.h"

/**
 *  This class is used by the Camera class to do the actual communication with
 *  the underlying libgphoto wrapper camera object. It is supposed to run in its
 *  own QThread so as not to block the application while communicating with the
 *  camera.
 */
class CameraController : public QObject
{
    Q_OBJECT
public:
    CameraController(Camera* const camera, QObject *parent = 0);
    ~CameraController();

signals:
    void nameChanged(const QString& name);

    void apertureChanged(const int newAperture);
    void shutterChanged(const int newShutter);
    void isoChanged(const int newIso);

    void apertureChoicesChanged(const QStringList& newApertureChoices);
    void shutterChoicesChanged(const QStringList& newShutterChoices);
    void isoChoicesChanged(const QStringList& newIsoChoices);

    void viewfinderChanged(const bool on);

    void newPreviewImage(QSharedPointer<Image> preview);
    void newImage(QSharedPointer<Image> image);

    void previewStarted();
    void previewStopped();

    void resetDone();
public slots:
    void readConfig();
    void readAperture();
    void readShutter();
    void readIso();
    void startPreview();
    void stopPreview();

    void setAperture(const int index);
    void setShutter(const int index);
    void setIso(const int index);

    void setViewfinder(bool on);

    void trigger();
    void readImage(const QFileInfo& fileInfo);

    void reset(); // releases UI lock
private slots:
    void capturePreview();
private:
    Camera *m_camera;
    std::shared_ptr<gp::Camera> m_gp_camera;
    volatile bool m_previewRunning;
    template <class Obj>
    void setRadioConfig(int value);
};
