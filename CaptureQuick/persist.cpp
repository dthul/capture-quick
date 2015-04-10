#include "persist.h"

#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

void Persist::saveImagesToDisk(QList<Camera*> const& cameras) {
    const QDateTime now(QDateTime::currentDateTimeUtc());
    auto destinationFolders = getDestinationFolders(cameras);
    for (int i = 0; i < cameras.length(); ++i) {
        auto camera = cameras[i];
        if (!camera->latestImage().isNull()) {
            // Make sure that the destination directory exists
            if (!QDir::root().mkpath(destinationFolders[i]))
                // TODO: throw error instead
                continue;
            const QDir destinationFolder(destinationFolders[i]);
            // Use the current time as the filename
            const QString fileName = QString::number(now.toMSecsSinceEpoch()) + ".jpg";
            if (destinationFolder.exists(fileName))
                // TODO: throw error instead
                continue;
            // Concatenate the folder and the file name
            const QString filePath = destinationFolder.filePath(fileName);
            camera->saveImage(filePath);
        }
    }
}

QList<QString> Persist::getDestinationFolders(QList<Camera*> const& cameras) {
    QList<QString> destinationFolders;
    QSettings settings;
    const QString defaultCaptureLocation =
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString captureRoot = settings.value("capture/root_path", defaultCaptureLocation).toString();
    // There is no function in Qt to concatenate paths (wtf!) so we hackishly do it ourselves
    const bool addSeparator = !defaultCaptureLocation.endsWith(QDir::separator());

    for (auto camera : cameras) {
        const QString cameraName = camera->name();
        const QString destinationFolder = captureRoot + (addSeparator ? QDir::separator() : QString("")) + cameraName + QDir::separator();
        destinationFolders.append(destinationFolder);
    }

    return destinationFolders;
}
