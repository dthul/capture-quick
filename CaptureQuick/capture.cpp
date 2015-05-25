#include "capture.h"

#include <iostream>
#include <cmath>
#include <stdlib.h>

#include <QQmlContext>
#include <QRunnable>
#include <QSettings>
#include <QStandardPaths>
#include <QThreadPool>
#include <QVariant>

#include "persist.h"

Capture::Capture(QObject *parent) :
    QObject(parent),
    m_num_captured(0),
    m_triggerBox(TriggerBox::getInstance()),
    // m_triggerBoxThread(new QThread()),
    m_num_rows(1),
    m_num_cols(1),
    m_all_camera_names_known(true),
    m_connected(false)
{ }

void Capture::connect() {
    if (m_connected)
        return;
#ifdef __APPLE__
    system("killall PTPCamera");
#endif
    m_gp_cameras = gpcontext.all_cameras();
    std::cout << "Found " << m_gp_cameras.size() << " cameras" << std::endl;
    if (m_gp_cameras.size() == 0)
        return;
    for (auto& gp_camera : m_gp_cameras) {
        Camera* camera = new Camera(gp_camera);
        m_cameras.append(camera);
        QObject::connect(camera, &Camera::imageChanged, this, &Capture::newImageCaptured);
        QObject::connect(camera, &Camera::rawImageChanged, this, &Capture::newImageCaptured);
        QObject::connect(camera, &Camera::nameChanged, this, &Capture::cameraNameChanged);
        QObject::connect(camera, &Camera::stateChanged, this, &Capture::cameraStateChanged);
        camera->readConfig();
    }
    if (m_cameras.size() > 0) {
        m_all_camera_names_known = false;
        emit allConfiguredChanged(false);
    }
    emit allCamerasChanged();
    m_connected = true;
}

class DeleteCameraTask : public QRunnable
{
public:
    DeleteCameraTask(Camera* const camera) : camera(camera) {}
private:
    void run() { delete camera; }
    Camera* const camera;
};

void Capture::disconnect() {
    if (!m_connected)
        return;

    // Destroys Capture's shared_ptrs to the gp::Cameras
    m_gp_cameras.clear();

    // The uiCamerasChanged and allCamerasChanged signals must be emitted before
    // actually deleting the underlying Cameras, otherwise QML will hold pointers
    // to deleted objects which ends badly.
    // That's why we keep a copy of m_cameras here to be able to delete them later.
    const QList<Camera*> cameras(m_cameras);
    m_cameras.clear();
    m_ui_cameras.clear();
    m_all_camera_names_known = true;
    m_num_rows = 1;
    m_num_cols = 1;
    m_num_captured = 0;
    m_connected = false;
    emit uiCamerasChanged();
    emit allCamerasChanged();
    emit allConfiguredChanged(true);
    emit numRowsChanged(m_num_rows);
    emit numColsChanged(m_num_cols);
    emit numCapturedChanged(m_num_captured);

    // Destroys the Camera objects and thereby the remaining shared_ptrs
    // to the gp::Cameras.
    // This will actually release the cameras and takes some time
    // so do it in parallel.
    // (instead of just doing cameras.clear())
    QThreadPool::globalInstance()->setMaxThreadCount(cameras.size());
    for (auto camera : cameras)
        QThreadPool::globalInstance()->start(new DeleteCameraTask(camera));
    QThreadPool::globalInstance()->waitForDone(8 * 1000);
}

Capture::~Capture() {
    disconnect();
}

int Capture::numCaptured() const {
    return m_num_captured;
}

void Capture::newCapture() {
    Persist::getInstance()->next();
    for (auto camera : m_cameras)
        camera->clearLatestImage();
}

void Capture::saveCaptureToDisk() {
    std::cout << "Saving capture to disk" << std::endl;
    const bool save_jpeg = saveJpeg();
    const bool save_raw = saveRaw();
    for (Camera * camera : m_cameras) {
        if (save_jpeg)
            camera->image()->save();
        if (save_raw)
            camera->rawImage()->save();
    }
}

void Capture::newImageCaptured(Image* image) {
    int num_captured = 0;
    for (auto camera : m_cameras) {
        if (!camera->image()->toQImage().isNull())
            ++num_captured;
    }
    m_num_captured = num_captured;
    emit numCapturedChanged(m_num_captured);
    if (autoSave()) {
        if ((!image->is_raw() && saveJpeg()) || (image->is_raw() && saveRaw()))
            image->save();
    }
}

QString Capture::captureRoot() const {
    QSettings settings;
    const QString defaultCaptureLocation =
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    return settings.value("capture/root_path", defaultCaptureLocation).toString();
}

void Capture::setCaptureRoot(const QString& newCaptureRoot) {
    QSettings settings;
    settings.setValue("capture/root_path", newCaptureRoot);
    emit captureRootChanged(newCaptureRoot);
}

bool Capture::autoSave() const {
    QSettings settings;
    return settings.value("capture/auto_save", false).toBool();
}

void Capture::setAutoSave(bool autoSave) {
    QSettings settings;
    settings.setValue("capture/auto_save", autoSave);
    emit autoSaveChanged(autoSave);
}

bool Capture::saveJpeg() const {
    QSettings settings;
    return settings.value("capture/save_jpeg", true).toBool();
}

void Capture::setSaveJpeg(bool saveJpeg) {
    QSettings settings;
    settings.setValue("capture/save_jpeg", saveJpeg);
    emit saveJpegChanged(saveJpeg);
}

bool Capture::saveRaw() const {
    QSettings settings;
    return settings.value("capture/save_raw", false).toBool();
}

void Capture::setSaveRaw(bool saveRaw) {
    QSettings settings;
    settings.setValue("capture/save_raw", saveRaw);
    emit saveRawChanged(saveRaw);
}

void Capture::halfPressShutterAll(const int milliseconds) {
    QMetaObject::invokeMethod(m_triggerBox, "halfPressShutterAll", Qt::QueuedConnection, Q_ARG(int, milliseconds));
}

void Capture::pressShutterAll(const int milliseconds) {
    QMetaObject::invokeMethod(m_triggerBox, "pressShutterAll", Qt::QueuedConnection, Q_ARG(int, milliseconds));
}

void Capture::cameraStateChanged() {
    emit commonStateChanged();
}

Camera::CameraState Capture::commonState() const {
    bool allPreview = true;
    bool allCapture = true;
    for (Camera* camera : m_cameras) {
        const Camera::CameraState state = camera->state();
        switch (state) {
        case Camera::CAMERA_CAPTURE:
            allPreview = false;
            break;
        case Camera::CAMERA_PREVIEW:
            allCapture = false;
            break;
        default:
            allPreview = false;
            allCapture = false;
        }
    }
    if (allPreview)
        return Camera::CAMERA_PREVIEW;
    else if (allCapture)
        return Camera::CAMERA_CAPTURE;
    else
        return Camera::CAMERA_NONE;
}

int Capture::countAllCameras(QQmlListProperty<Camera> *list) {
    Capture *capture = qobject_cast<Capture*>(list->object);
    if (capture) {
        return capture->m_cameras.length();
    }
    return 0;
}

Camera* Capture::atAllCameras(QQmlListProperty<Camera> *list, int index) {
    Capture *capture = qobject_cast<Capture*>(list->object);
    if (capture && index >= 0 && index < capture->m_cameras.size()) {
        return capture->m_cameras.at(index);
    }
    return nullptr;
}

QQmlListProperty<Camera> Capture::allCameras() {
    return QQmlListProperty<Camera>(this, 0, &Capture::countAllCameras, &Capture::atAllCameras);
}

int Capture::countUiCameras(QQmlListProperty<Camera> *list) {
    Capture *capture = qobject_cast<Capture*>(list->object);
    if (capture) {
        return capture->m_ui_cameras.length();
    }
    return 0;
}

Camera* Capture::atUiCameras(QQmlListProperty<Camera> *list, int index) {
    Capture *capture = qobject_cast<Capture*>(list->object);
    if (capture && index >= 0 && index < capture->m_ui_cameras.size()) {
        return capture->m_ui_cameras.at(index);
    }
    return nullptr;
}

QQmlListProperty<Camera> Capture::uiCameras() {
    return QQmlListProperty<Camera>(this, 0, &Capture::countUiCameras, &Capture::atUiCameras);
}

int Capture::numRows() const {
    return m_num_rows;
}

int Capture::numCols() const {
    return m_num_cols;
}

void Capture::recalculateGridSize() {
    m_num_rows = std::floor(std::sqrt(m_ui_cameras.size()));
    m_num_cols = std::ceil(static_cast<float>(m_ui_cameras.size()) / m_num_rows);
    emit numRowsChanged(m_num_rows);
    emit numColsChanged(m_num_cols);
}

void Capture::loadCameraArrangementFromSettings() {
    QSettings settings;
    const QString arrangement = settings.value("capture/camera_arrangement").toString();
    loadCameraArrangement(arrangement);
}

void Capture::loadCameraArrangementFromFile(const QString& fileName) {
    QFile data(fileName);
    QString arrangement;
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        arrangement = in.readAll();
    }
    else {
        emit alert("Could not open file");
        return;
    }
    loadCameraArrangement(arrangement);
}

void Capture::writeCameraArrangementToFile(const QString& fileName) {
    QString arrangement = serializeCameraArrangement();
    QFile data(fileName);
    if (data.open(QFile::WriteOnly)) {
        QTextStream out(&data);
        out << arrangement;
        data.close();
    }
}

QString Capture::serializeCameraArrangement() {
    // TODO: throw error or return empty string when not serializable
    // (e.g. not all cameras have a name)
    QString arrangement;
    QTextStream out(&arrangement);
    out << m_num_rows << "\t" << m_num_cols << "\n";
    for (auto r = 0; r < m_num_rows; ++r) {
        for (auto c = 0; c < m_num_cols; ++c) {
            auto index = r * m_num_cols + c;
            Camera* camera = index < m_ui_cameras.size() ? m_ui_cameras[index] : nullptr;
            int rotation = index < m_ui_camera_rotations.size() ? m_ui_camera_rotations[index] : 0;
            if (camera) {
                out << camera->name();
                if (rotation != 0)
                    out << ":" << rotation;
            }
            else {
                out << "0";
            }
            if (c < m_num_cols - 1)
                out << "\t";
        }
        out << "\n";
    }
    return arrangement;
}

void Capture::loadCameraArrangement(QString arrangement) {
    if (!arrangement.isEmpty()) {
        QList<Camera*> ui_cameras;
        QList<int> ui_camera_rotations;
        int num_rows, num_cols;
        QTextStream in(&arrangement);
        // TODO: catch error when something during reading goes wrong
        // and set m_ui_cameras to m_cameras
        QString sizeLine = in.readLine();
        QTextStream size(&sizeLine);
        size >> num_rows;
        size >> num_cols;
        for (auto r = 0; r < num_rows; ++r) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty())
                continue;
            QStringList cameraSpecifiers = line.split(QRegExp("\\s+"));
            int numCamerasAdded = 0;
            for (auto& cameraSpecifier : cameraSpecifiers) {
                if (numCamerasAdded >= num_cols) {
                    std::cout << "warning: row " << r + 1 << " of the camera arrangement contains too many entries" << std::endl;
                    break;
                }
                QRegExp const specifierRegexp("^(\\w+)(?::(-?\\d+))?$");
                if (!specifierRegexp.exactMatch(cameraSpecifier)) {
                    emit alert("Invalid camera specifer '" + cameraSpecifier + "'. Resetting camera arrangement.");
                    resetCameraArrangement();
                    return;
                }
                QStringList const matches = specifierRegexp.capturedTexts();
                QString const cameraName = matches[1];
                int const rotation = matches.length() > 2 ? matches[2].toInt() : 0;
                Camera *camera = findCameraByName(cameraName);
                if (!camera && cameraName != "0") {
                    emit alert("Could not load camera arrangement. Camera '" + cameraName + "' is missing.\nResetting camera arrangement.");
                    resetCameraArrangement();
                    return;
                }
                ui_cameras.push_back(camera);
                ui_camera_rotations.push_back(rotation);
                ++numCamerasAdded;
            }
            // Was the line shorter than num_cols? Fill it up with nullptrs
            for (auto i = numCamerasAdded; i < num_cols; ++i) {
                ui_cameras.push_back(nullptr);
                ui_camera_rotations.push_back(0);
            }
        }
        m_num_rows = num_rows;
        m_num_cols = num_cols;
        emit numRowsChanged(num_rows);
        emit numColsChanged(num_cols);
        m_ui_cameras.clear();
        m_ui_cameras.append(ui_cameras);
        m_ui_camera_rotations.clear();
        m_ui_camera_rotations.append(ui_camera_rotations);
        emit uiCamerasChanged();
        emit uiCameraRotationsChanged();
        // m_ui_cameras = ui_cameras;
    }
    else {
        m_ui_cameras.clear();
        m_ui_cameras.append(m_cameras);
        emit uiCamerasChanged();
        // m_ui_cameras = m_cameras;
        recalculateGridSize();
    }
    QSettings settings;
    settings.setValue("capture/camera_arrangement", serializeCameraArrangement());
}

void Capture::resetCameraArrangement() {
    m_ui_cameras.clear();
    m_ui_cameras.append(m_cameras);
    m_ui_camera_rotations.clear();
    for (int _ = 0; _ < m_ui_cameras.length(); ++_)
        m_ui_camera_rotations.append(0);
    emit uiCamerasChanged();
    emit uiCameraRotationsChanged();
    // m_ui_cameras = m_cameras;
    recalculateGridSize();
    QSettings settings;
    settings.setValue("capture/camera_arrangement", serializeCameraArrangement());
}

Camera* Capture::findCameraByName(const QString& name) {
    for (auto camera : m_cameras)
        if (camera->name() == name)
            return camera;
    return nullptr;
}

void Capture::cameraNameChanged(const QString& /*name*/) {
    for (auto camera : m_cameras)
        if (camera->name().isEmpty()) {
            emit allConfiguredChanged(false);
            return;
        }
    // We know all camera's names now
    m_all_camera_names_known = true;
    emit allConfiguredChanged(true);
    loadCameraArrangementFromSettings();
}

bool Capture::allConfigured() const {
    return m_all_camera_names_known;
}

QList<int> Capture::uiCameraRotations() {
    return m_ui_camera_rotations;
}
