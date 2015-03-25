#pragma once

#include <QImage>
#include <QObject>
#include <QStringList>

#include "gputil.h"

class CameraController : public QObject
{
    Q_OBJECT
    typedef std::vector<char> JpegBuffer;
public:
    CameraController(gp::Camera *camera, QObject *parent = 0);
    ~CameraController();

signals:
    void nameChanged(const QString& name);

    void apertureChanged(const int newAperture);
    void shutterChanged(const int newShutter);
    void isoChanged(const int newIso);

    void apertureChoicesChanged(const QStringList& newApertureChoices);
    void shutterChoicesChanged(const QStringList& newShutterChoices);
    void isoChoicesChanged(const QStringList& newIsoChoices);

    void newPreviewImage(QImage preview);

    void previewStarted();
    void previewStopped();
public slots:
    void readConfig();
    void startPreview();
    void stopPreview();

    void setAperture(const int index);
    void setShutter(const int index);
    void setIso(const int index);
private slots:
    void capturePreview();
private:
    gp::Camera *m_camera;
    volatile bool m_previewRunning;
    template <class Obj>
    void setRadioConfig(int value);
};
