#include "cameraeventlistener.h"

#include <iostream>

#include <QDir>
#include <QMetaObject>
#include <QRegExp>
#include <QStringList>
#include <QThread>

CameraEventListener::CameraEventListener(std::shared_ptr<gp::Camera> const camera, QObject *parent) :
    QObject(parent),
    m_camera(camera),
    m_listen(true)
{
}

CameraEventListener::~CameraEventListener()
{

}

void CameraEventListener::startListening() {
    m_listen = true;
    QMetaObject::invokeMethod(this, "waitForEvent", Qt::QueuedConnection);
}

void CameraEventListener::stopListening() {
    m_listen = false;
}

void CameraEventListener::waitForEvent() {
    if (!m_listen)
        return;

    try {
        gp::CameraEvent ev = m_camera->wait_event(20);
        handleEvent(ev);
    } catch (gp::Exception& e) {
        std::cout << "CameraEventListener: " << e.what() << std::endl;
        QThread::sleep(1);
    }

    if (m_listen)
        QMetaObject::invokeMethod(this, "waitForEvent", Qt::QueuedConnection);
}

void CameraEventListener::handleEvent(const gp::CameraEvent& ev) {
    using Ce = gp::CameraEvent;
    const QRegExp ptp_property_regex("^PTP Property ([0-9a-f]{4}) changed$");

    /*
    if (ev.type() != Ce::EVENT_TIMEOUT && cfg.verboselevel >= VERBOSE_TALKATIVE) {
        if (ev.type() != Ce::EVENT_UNKNOWN || cfg.verboselevel == VERBOSE_HIGH)
            std::cout << "Cam " << name << "#" << evts << ": "
                << ev.type() << ": " << ev.typestr();
    }
    */

    switch (ev.type()) {
    case Ce::EVENT_UNKNOWN: {
        // Try to extract a PTP property number from the event and see if we know it
        if(!ptp_property_regex.exactMatch(QString::fromStdString(ev.get<Ce::EVENT_UNKNOWN>())))
            break;
        const QString property = ptp_property_regex.capturedTexts()[1];
        // d1d3: shutter was pressed?
        if (property == "d1d9" || property == "d1d5" || property == "d1b0") {
            // ignore
        }
        else if (property == "d101") {
            // Aperture changed
            emit apertureChanged();
        }
        else if (property == "d102") {
            // Shutter speed changed
            emit shutterChanged();
        }
        else if (property == "d103" || property == "d11b") { // TODO: both?
            // ISO changed
            emit isoChanged();
        }
        else {
            // std::cout << "got event " << property.toStdString() << std::endl;
        }
        }
        break;
    case Ce::EVENT_TIMEOUT:
        // no event, no problem
        break;
    case Ce::EVENT_FILE_ADDED: {
        auto pathinfo = ev.get<Ce::EVENT_FILE_ADDED>();
        QFileInfo fileInfo(
                    QString::fromStdString(pathinfo.first),
                    QString::fromStdString(pathinfo.second));
        emit newImageAdded(fileInfo);
        }
        break;
    case Ce::EVENT_FOLDER_ADDED: {
        /*auto pathinfo = ev.get<Ce::EVENT_FOLDER_ADDED>();
        if (cfg.verboselevel >= VERBOSE_TALKATIVE) {
            std::cout << ": " << pathinfo.first << " "
                    << pathinfo.second << std::endl;
        }*/
        }
        break;
    case Ce::EVENT_CAPTURE_COMPLETE:
        // wat
        /*if (cfg.verboselevel >= VERBOSE_TALKATIVE) {
            std::cout << "(?)" << std::endl;
        }*/
        break;
    }
}
