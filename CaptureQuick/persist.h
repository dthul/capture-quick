#pragma once

#include <QList>
#include <QString>

#include "camera.h"

class Persist
{
    Persist() {}
public:
    static void saveImagesToDisk(QList<Camera*> const& cameras, QString prefix);
    static QList<QString> getDestinationFolders(QList<Camera*> const& cameras);
};
