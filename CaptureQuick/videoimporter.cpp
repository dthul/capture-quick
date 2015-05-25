#include "videoimporter.h"

VideoImporter::VideoImporter(Capture *const capture, QObject *parent) :
    QObject(parent),
    m_capture(capture) {}

VideoImporter::~VideoImporter()
{

}

