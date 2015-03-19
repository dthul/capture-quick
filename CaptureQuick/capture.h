#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "camera.h"
#include "gputil.h"

class Capture : public QObject
{
    Q_OBJECT
public:
    Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent = 0);
    ~Capture();

signals:

public slots:
private:
    QQmlApplicationEngine* const m_qml_engine;
    QList<QObject*> m_cameras;
    gp::Context gpcontext;
    std::vector<gp::Camera> m_gp_cameras;
};
