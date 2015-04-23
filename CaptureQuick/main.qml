import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import CaptureQuick 0.1 // Makes the Camera class available (registered in main.cpp)

import "qrc:/fontawesome.js" as FontAwesome

ApplicationWindow {
    id: app
    title: qsTr("Capture Quick")
    width: 1920
    height: 1080
    visible: true

    function extractPathFromURL(url) {
        // More or less hacky way to extract the absolute path from
        // the URL that the FileDialog returns.
        // see: http://stackoverflow.com/a/26868237/850264
        // remove prefix "file://"
        url = url.toString()
        var path = url.replace(/^(file:\/{2})/,"")
        // unescape HTML codes like '%23'
        path = decodeURIComponent(path)
        return path
    }

    FontLoader {
        source: "qrc:/fontawesome-webfont.ttf"
    }

    FileDialog {
        id: cameraArrangementLoadFileDialog
        nameFilters: ["Capture Quick Arrangements (*.cqa)", "All files (*)"]
        title: "Load Camera Arrangement"
        onAccepted: {
            var path = extractPathFromURL(this.fileUrl)
            console.log("Loading camera arrangement from file...")
            capture.loadCameraArrangementFromFile(path)
        }
    }

    FileDialog {
        id: cameraArrangementWriteFileDialog
        nameFilters: ["Capture Quick Arrangements (*.cqa)", "All files (*)"]
        title: "Save Camera Arrangement"
        selectExisting: false
        onAccepted: {
            var path = extractPathFromURL(this.fileUrl)
            console.log("Saving camera arrangement to file...")
            capture.writeCameraArrangementToFile(path)
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                shortcut: "Ctrl+O"
                text: qsTr("L&oad Camera Arrangement")
                onTriggered: cameraArrangementLoadFileDialog.open()
            }
            MenuItem {
                shortcut: "Ctrl+S"
                text: qsTr("S&ave Camera Arrangement")
                onTriggered: cameraArrangementWriteFileDialog.open()
            }
            MenuItem {
                shortcut: "Ctrl+R"
                text: qsTr("&Reset Camera Arrangement")
                onTriggered: capture.resetCameraArrangement()
            }

            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit()
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            anchors.fill: parent
            Label { text: capture.allCameras.length + " cameras connected" }
            Label { text: capture.allConfigured ? "" : "Waiting for camera configurations" }
            Label { text: capture.numRows + "x" + capture.numCols }
        }
    }

    Item {
        id: mainFrame
        anchors.fill: parent

        RowLayout {
            id: rowLayout1
            anchors.fill: parent
            anchors.margins: 5

/*            GridLayout {
                id: imageGrid
                columns: 3
                rows: 3
                columnSpacing: 5
                rowSpacing: 5

                ImageView {
                    id: imageView0
                    model: capture.allCameras[0]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
*/
            GridView {
                id: imageGrid
                model: capture.uiCameras
                cellWidth: width / capture.numCols
                cellHeight: height / capture.numRows
                Layout.fillWidth: true
                Layout.fillHeight: true
                delegate: Component {
                    Loader {
                        id: imageLoader
                        height: imageGrid.cellHeight
                        width: imageGrid.cellWidth
                        source: capture.uiCameras[index] ? "ImageView.qml" : "EmptyImageView.qml"
                    }
                }
            }

            ColumnLayout {
                id: columnLayout1
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: 0.15 * parent.width

                Label {
                    text: "1. Set up cameras"
                    font.bold: true
                }
                Button {
                    text: qsTr("Activate Live Preview Mode")
                    onClicked: {
                        for (var i = 0; i < capture.allCameras.length; ++i) {
                            capture.allCameras[i].state = Camera.CAMERA_PREVIEW
                        }
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                Label {
                    text: "2. Configure Capture"
                    font.bold: true
                }
                FileDialog {
                    id: captureRootDialog
                    title: qsTr("Choose a location for the captured images")
                    folder: capture.captureRoot
                    selectFolder: true
                    onAccepted: {
                        var path = extractPathFromURL(this.folder)
                        capture.captureRoot = path
                    }
                }
                Label {
                    text: "Destination directory:"
                }
                RowLayout {
                    id: rowLayout2
                    Text {
                        Layout.fillWidth: true
                        text: capture.captureRoot
                        elide: Text.ElideLeft
                        MouseArea {
                            id: captureRootHover
                            anchors.fill: parent
                            hoverEnabled: true
                            property bool hovered: false
                            onEntered: hovered = true
                            onExited: hovered = false
                        }
                    }
                    Text {
                        id: settings
                        font.pointSize: 15
                        font.family: "FontAwesome"
                        text: FontAwesome.Icon.Cog
                        MouseArea {
                            cursorShape: Qt.PointingHandCursor
                            anchors.fill: parent
                            onClicked: captureRootDialog.open()
                        }
                    }
                }
                CheckBox {
                    id: autoSaveCheckbox
                    text: "Auto Save"

                    checked: capture.autoSave
                    onCheckedChanged: {
                        if (checked != capture.autoSave) {
                            capture.autoSave = checked;
                        }
                    }
                }
                Label {
                    text: "3. Capture!"
                    font.bold: true
                }
                Button {
                    text: qsTr("Activate Capture Mode")
                    //id: button1
                    onClicked: {
                        for (var i = 0; i < capture.allCameras.length; ++i) {
                            capture.allCameras[i].state = Camera.CAMERA_CAPTURE
                        }
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                Button {
                    text: qsTr("Capture")
                    onClicked: {
                        capture.newCapture()
                        capture.triggerAll()
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                Button {
                    text: qsTr("Save captured images")
                    onClicked: capture.saveCaptureToDisk()
                    enabled: !autoSaveCheckbox.checked
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                Text {
                    text: capture.numCaptured + " / " + capture.allCameras.length
                }
                Button {
                    text: "Focus"
                    onClicked: capture.focusAll()
                }
            }
        }

        Tooltip {
            text: "Saves the captured images as\nsoon as every camera triggered"
            opacity: autoSaveCheckbox.hovered ? 0.8 : 0
            // Hacky hack hack
            x: autoSaveCheckbox.mapToItem(mainFrame, 0 * columnLayout1.x * autoSaveCheckbox.x, 0).x + autoSaveCheckbox.width / 2 - width / 2
            y: autoSaveCheckbox.mapToItem(mainFrame, 0, 0 * columnLayout1.y * autoSaveCheckbox.y).y + autoSaveCheckbox.height + 5
            z: 9999
        }
        Tooltip {
            text: capture.captureRoot
            opacity: captureRootHover.hovered ? 0.8 : 0
            // Hacky hack hack
            x: captureRootHover.mapToItem(mainFrame, 0 * rowLayout2.x * captureRootHover.x, 0).x + captureRootHover.width / 2 - width / 2
            y: captureRootHover.mapToItem(mainFrame, 0, 0 * rowLayout2.y * captureRootHover.y).y + captureRootHover.height + 5
            z: 9999
        }
    }

    function broadcastSettings(sourceCamera) {
        for (var i = 0; i < capture.allCameras.length; ++i) {
            var destCamera = capture.allCameras[i];
            if (sourceCamera !== destCamera) {
                destCamera.apertureIndex = sourceCamera.apertureIndex;
                destCamera.shutterIndex = sourceCamera.shutterIndex;
                destCamera.isoIndex = sourceCamera.isoIndex;
            }
        }
    }

    /*function reloadImageGrid() {
        var newChild = Qt.createComponent("ImageView.qml")
        Qt.createQmlObject()
        newChild.model = capture.allCameras[0]
        newChild.Layout.fillHeight = true
        newChild.Layout.fillWidth = true
        imageGrid.children = [newChild]
    }*/

    MessageDialog {
        id: alertDialog
    }

    Connections {
        target: capture
        onAlert: {
            // TODO: queue alert messages?
            alertDialog.text = message
            alertDialog.open()
        }
    }
}
