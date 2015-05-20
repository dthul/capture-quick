#pragma once

#include <QDateTime>
#include <QList>
#include <QMutex>
#include <QString>

#include "image.h"

class Persist
{
    Persist();
public:
    static Persist* getInstance();
    void save(Image* const image);
    void next();
private:
    struct FileName {
        QString dir;
        QString name;
        QString path() const;
    };
    FileName fileNameFor(Image* const image);
    static Persist* persist;
    qint64 m_time;
    static QMutex s_mutex;
    mutable QMutex m_mutex;
};
