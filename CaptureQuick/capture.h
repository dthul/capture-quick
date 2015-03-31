#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "camera.h"
#include "gputil.h"
#include "liveimageprovider.h"

class Capture : public QObject
{
    Q_OBJECT
private:
    Q_PROPERTY(int numCaptured READ numCaptured NOTIFY numCapturedChanged)
public:
    Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent = 0);
    ~Capture();

    int numCaptured() const;
signals:
    void numCapturedChanged(const int numCaptured);
public slots:
    void newCapture();
    void saveCaptureToDisk();
private slots:
    void newImageCaptured();
private:
    QQmlApplicationEngine* const m_qml_engine;
    QList<QObject*> m_cameras;
    gp::Context gpcontext;
    std::vector<gp::Camera> m_gp_cameras;
    int m_num_captured;
};
