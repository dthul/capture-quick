#include "capture.h"

#include <iostream>

#include <QQmlContext>
#include <QVariant>

Capture::Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent) :
    QObject(parent),
    m_qml_engine(qmlEngine)
    //m_live_image_provider(reinterpret_cast<QList<Camera*>*>(&m_cameras))
{
    /*
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    */

    m_gp_cameras = gpcontext.all_cameras();
    std::cout << "Found " << m_gp_cameras.size() << " cameras" << std::endl;
    for (auto& gp_camera : m_gp_cameras) {
        Camera* camera = new Camera(&gp_camera);
        m_cameras.append(camera);
    }

    for (QObject* camera : m_cameras) {
        static_cast<Camera*>(camera)->startPreview();
    }

    // will be freed by Qt
    LiveImageProvider *liveImgProvider = new LiveImageProvider(reinterpret_cast<QList<Camera*>*>(&m_cameras));
    m_qml_engine->addImageProvider("live", liveImgProvider);

    m_qml_engine->rootContext()->setContextProperty("cameras", QVariant::fromValue(m_cameras));
}

Capture::~Capture()
{
    for (QObject* camera : m_cameras) {
        static_cast<Camera*>(camera)->stopPreview();
    }
    for (QObject* camera : m_cameras) {
        delete camera;
    }
    m_cameras.clear();
    // This is the call that actually releases the cameras
    // by destroying the underlying gp::Cameras
    m_gp_cameras.clear();
}
