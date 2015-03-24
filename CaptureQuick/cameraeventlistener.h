#pragma once

#include <QObject>

#include "gputil.h"

class CameraEventListener : public QObject
{
    Q_OBJECT
public:
    CameraEventListener(gp::Camera *camera, QObject *parent = 0);
    ~CameraEventListener();

signals:

public slots:
    void startListening();
    void stopListening();
private slots:
    void waitForEvent();
private:
    void handleEvent(const gp::CameraEvent& ev);
    gp::Camera *m_camera = nullptr;
    volatile bool m_listen;
};
