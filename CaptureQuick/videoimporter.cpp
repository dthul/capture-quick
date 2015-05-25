#include "videoimporter.h"

VideoImporter::VideoImporter(Capture *const capture, QObject *parent = 0) :
    QObjectCleanupHandler(parent),
    m_capture(capture) {}

VideoImporter::~VideoImporter()
{

}

