#pragma once

#include <QImage>
#include <QObject>

#include "gputil.h"

class CameraController : public QObject
{
    Q_OBJECT
    typedef std::vector<char> JpegBuffer;
public:
    CameraController(gp::Camera *camera, QObject *parent = 0);
    ~CameraController();

signals:
    void apertureChanged(const int newAperture);
    void shutterChanged(const int newShutter);
    void isoChanged(const int newIso);

    void apertureChoicesChanged(const QList<QString>& newApertureChoices);
    void shutterChoicesChanged(const QList<QString>& newShutterChoices);
    void isoChoicesChanged(const QList<QString>& newIsoChoices);

    void newPreviewImage(QImage preview);

    void previewStarted();
    void previewStopped();
public slots:
    void readConfig();
    void startPreview();
    void stopPreview();
private slots:
    void capturePreview();
private:
    gp::Camera *m_camera;
    volatile bool m_previewRunning;
};
