#include "capture.h"

#include <iostream>
#include <cmath>

#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>

#include "persist.h"

Capture::Capture(QQmlApplicationEngine* const qmlEngine, QObject *parent) :
    QObject(parent),
    m_qml_engine(qmlEngine),
    m_num_captured(0),
    m_triggerBox(new TriggerBox()),
    m_triggerBoxThread(new QThread()),
    m_all_camera_names_known(false)
{
    QSettings settings;
    const QString defaultCaptureLocation =
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    m_capture_root = settings.value("capture/root_path", defaultCaptureLocation).toString();
    m_auto_save = settings.value("capture/auto_save", false).toBool();
    m_gp_cameras = gpcontext.all_cameras();
    std::cout << "Found " << m_gp_cameras.size() << " cameras" << std::endl;
    for (auto& gp_camera : m_gp_cameras) {
        Camera* camera = new Camera(&gp_camera);
        m_cameras.append(camera);
        connect(camera, &Camera::imageUrlChanged, this, &Capture::newImageCaptured);
        connect(camera, &Camera::nameChanged, this, &Capture::cameraNameChanged);
        camera->readConfig();
    }

    m_triggerBox->moveToThread(m_triggerBoxThread);
    std::cout << "triggerBoxThread: " << std::hex << m_triggerBoxThread << std::endl;
    connect(m_triggerBoxThread, &QThread::started, m_triggerBox, &TriggerBox::init);
    m_triggerBoxThread->start();

    // will be freed by Qt
    LiveImageProvider *liveImgProvider = new LiveImageProvider(&m_cameras);
    m_qml_engine->addImageProvider("live", liveImgProvider);

    m_qml_engine->rootContext()->setContextProperty("capture", this);
}

Capture::~Capture()
{
    // TODO: somehow destroy the gp::Cameras in parallel, since destroying
    // them sequentially is unnecessary and takes a lot of time
    // HACK for now: first switch all cameras to capture mode.
    // This speeds the process up a little
    for (auto camera : m_cameras)
        camera->setState(Camera::CameraState::CAMERA_CAPTURE);

    if (m_triggerBoxThread) {
        m_triggerBoxThread->quit();
        m_triggerBoxThread->wait(3);
    }
    delete m_triggerBox;
    delete m_triggerBoxThread;

    for (auto camera : m_cameras)
        delete camera;
    m_cameras.clear();
    // This is the call that actually releases the cameras
    // by destroying the underlying gp::Cameras
    m_gp_cameras.clear();
}

int Capture::numCaptured() const {
    return m_num_captured;
}

void Capture::newCapture() {
    m_capture_time = QDateTime::currentDateTimeUtc();
    for (auto camera : m_cameras)
        camera->clearLatestImage();
}

void Capture::saveCaptureToDisk() {
    std::cout << "Saving capture to disk" << std::endl;
    Persist::saveImagesToDisk(m_cameras, QString::number(m_capture_time.toMSecsSinceEpoch()));
}

void Capture::newImageCaptured() {
    int num_captured = 0;
    for (auto camera : m_cameras) {
        if (!camera->latestImage().isNull())
            ++num_captured;
    }
    m_num_captured = num_captured;
    emit numCapturedChanged(m_num_captured);
    if (m_auto_save && m_num_captured == m_cameras.length())
        saveCaptureToDisk();
}

QString Capture::captureRoot() const {
    return m_capture_root;
}

void Capture::setCaptureRoot(const QString& newCaptureRoot) {
    QSettings settings;
    m_capture_root = newCaptureRoot;
    settings.setValue("capture/root_path", m_capture_root);
    emit captureRootChanged(m_capture_root);
}

bool Capture::autoSave() const {
    return m_auto_save;
}

void Capture::setAutoSave(bool autoSave) {
    QSettings settings;
    m_auto_save = autoSave;
    settings.setValue("capture/auto_save", m_auto_save);
    emit autoSaveChanged(m_auto_save);
}

void Capture::focusAll() {
    QMetaObject::invokeMethod(m_triggerBox, "focusAll", Qt::QueuedConnection);
}

void Capture::triggerAll() {
    QMetaObject::invokeMethod(m_triggerBox, "triggerAll", Qt::QueuedConnection);
}

QQmlListProperty<Camera> Capture::allCameras() {
    // TODO: replace with production grade QQmlListProperty (see Qt documentation)
    return QQmlListProperty<Camera>(this, m_cameras);
}

int Capture::countUiCameras(QQmlListProperty<Camera> *property) {
    return reinterpret_cast<Capture*>(property->data)->m_ui_cameras.length();
}

Camera* Capture::atUiCameras(QQmlListProperty<Camera> *property, int index) {
    return reinterpret_cast<Capture*>(property->data)->m_ui_cameras.at(index);
}

QQmlListProperty<Camera> Capture::uiCameras() {
    // TODO: replace with production grade QQmlListProperty (see Qt documentation)
    // return QQmlListProperty<Camera>(this, m_ui_cameras);
    return QQmlListProperty<Camera>(this, this, &Capture::countUiCameras, &Capture::atUiCameras);
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
            Camera* camera = index < m_cameras.size() ? m_cameras[index] : nullptr;
            if (camera)
                out << camera->name();
            else
                out << "0";
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
            QStringList cameraNames = line.split(QRegExp("\\s+"));
            int numCamerasAdded = 0;
            for (auto& cameraName : cameraNames) {
                if (numCamerasAdded >= num_cols) {
                    std::cout << "warning: row " << r + 1 << " of the camera arrangement contains too many entries" << std::endl;
                    break;
                }
                Camera *camera = findCameraByName(cameraName);
                if (!camera && cameraName != "0") {
                    emit alert("Could not load camera arrangement. Camera '" + cameraName + "' is missing.\nResetting camera arrangement...");
                    resetCameraArrangement();
                    return;
                }
                ui_cameras.push_back(camera);
                ++numCamerasAdded;
            }
            // Was the line shorter than num_cols? Fill it up with nullptrs
            for (auto i = numCamerasAdded; i < num_cols; ++i)
                ui_cameras.push_back(nullptr);
        }
        m_num_rows = num_rows;
        m_num_cols = num_cols;
        emit numRowsChanged(num_rows);
        emit numColsChanged(num_cols);
        m_ui_cameras = ui_cameras;
    }
    else {
        m_ui_cameras = m_cameras;
        recalculateGridSize();
    }
    emit uiCamerasChanged(uiCameras());
    QSettings settings;
    settings.setValue("capture/camera_arrangement", serializeCameraArrangement());
}

void Capture::resetCameraArrangement() {
    m_ui_cameras = m_cameras;
    emit uiCamerasChanged(uiCameras());
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
