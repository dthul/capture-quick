#ifndef VIDEOIMPORTER_H
#define VIDEOIMPORTER_H

#include <QObject>

class VideoImporter : public QObject
{
    Q_OBJECT
public:
    explicit VideoImporter(QObject *parent = 0);
    ~VideoImporter();

signals:

public slots:
};

#endif // VIDEOIMPORTER_H
