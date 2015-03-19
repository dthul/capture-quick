#include "capture.h"

#include <iostream>

#include <QQmlContext>
#include <QVariant>

Capture::Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent) :
    QObject(parent),
    m_qml_engine(qmlEngine)
{
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);
    m_cameras.append(new Camera);

    m_gp_cameras = gpcontext.all_cameras();
    std::cout << "Found " << m_gp_cameras.size() << " cameras" << std::endl;
    for (auto& gp_camera : m_gp_cameras)
        m_cameras.append(new Camera(&gp_camera));

    m_qml_engine->rootContext()->setContextProperty("cameras", QVariant::fromValue(m_cameras));
}

Capture::~Capture()
{

}

