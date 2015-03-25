#include "cameraeventlistener.h"

#include <iostream>

#include <QMetaObject>
#include <QThread>

CameraEventListener::CameraEventListener(gp::Camera *camera, QObject *parent) :
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
        gp::CameraEvent ev = m_camera->wait_event(1);
        handleEvent(ev);
    } catch (gp::Exception& e) {
        QThread::sleep(1);
    }

    if (m_listen)
        QMetaObject::invokeMethod(this, "waitForEvent", Qt::QueuedConnection);
}

void CameraEventListener::handleEvent(const gp::CameraEvent& ev) {
    using Ce = gp::CameraEvent;

    /*
    if (ev.type() != Ce::EVENT_TIMEOUT && cfg.verboselevel >= VERBOSE_TALKATIVE) {
        if (ev.type() != Ce::EVENT_UNKNOWN || cfg.verboselevel == VERBOSE_HIGH)
            std::cout << "Cam " << name << "#" << evts << ": "
                << ev.type() << ": " << ev.typestr();
    }
    */
    //std::cout << "got event" << std::endl;

    switch (ev.type()) {
    case Ce::EVENT_UNKNOWN:
        /*if (cfg.verboselevel == VERBOSE_HIGH)
            std::cout << ": " << ev.get<Ce::EVENT_UNKNOWN>() << std::endl;*/
        break;
    case Ce::EVENT_TIMEOUT:
        // no event, no problem
        break;
    case Ce::EVENT_FILE_ADDED: {
        auto pathinfo = ev.get<Ce::EVENT_FILE_ADDED>();
        /*if (cfg.verboselevel >= VERBOSE_TALKATIVE) {
            std::cout << ": " << pathinfo.first << " "
                    << pathinfo.second << std::endl;
        }
        std::string localdest = local_filename(pathinfo.second, cfg, name, locks.sequenceid);
        // camera folder and file name separately
        cam.save_file(pathinfo.first, pathinfo.second, localdest, cfg.delete_on_download);
        if (cfg.verboselevel >= VERBOSE_NORMAL)
            std::cout << name << " downloaded " << pathinfo.second
                << " -> " << localdest << "..." << std::endl;

        locks.readycount.notify_one();
        // TODO: download queue and try_wait, not to clog gphoto event
        // queue? probably all cameras have pretty same speed, so it's
        // just unnecessarily complicated
        locks.usernotified.wait();*/
        }
        break;
    case Ce::EVENT_FOLDER_ADDED: {
        auto pathinfo = ev.get<Ce::EVENT_FOLDER_ADDED>();
        /*if (cfg.verboselevel >= VERBOSE_TALKATIVE) {
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
