#pragma once

#include <QObject>

#include "capture.h"

class VideoImporter : public QObject
{
    Q_OBJECT
public:
    explicit VideoImporter(Capture *const capture, QObject *parent = 0);
    ~VideoImporter();

signals:

public slots:
private:
    Capture *const m_capture;
};
