#pragma once

#include <memory>

#include <QFileInfo>
#include <QObject>

#include "gputil.h"

class CameraEventListener : public QObject
{
    Q_OBJECT
public:
    CameraEventListener(std::shared_ptr<gp::Camera> const camera, QObject *parent = 0);
    ~CameraEventListener();

signals:
    void apertureChanged();
    void shutterChanged();
    void isoChanged();
    void newImageAdded(const QFileInfo& fileInfo);
public slots:
    void startListening();
    void stopListening();
private slots:
    void waitForEvent();
private:
    void handleEvent(const gp::CameraEvent& ev);
    std::shared_ptr<gp::Camera> m_camera = nullptr;
    volatile bool m_listen;
};
