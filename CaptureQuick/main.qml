import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import CaptureQuick 0.1 // Makes the Camera class available (registered in main.cpp)

import "qrc:/fontawesome.js" as FontAwesome

ApplicationWindow {
    id: app
    title: qsTr("Hello World")
    width: 1920
    height: 1080
    visible: true

    FontLoader {
        source: "qrc:/fontawesome-webfont.ttf"
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    Item {
        id: mainFrame
        anchors.fill: parent

        RowLayout {
            id: rowLayout1
            anchors.fill: parent
            anchors.margins: 5

            GridLayout {
                id: imageGrid
                columns: 3
                rows: 3
                columnSpacing: 5
                rowSpacing: 5

                ImageView {
                    id: imageView0
                    model: cameras[0]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView1
                    model: cameras[1]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView2
                    model: cameras[2]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView3
                    model: cameras[3]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView4
                    model: cameras[4]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView5
                    model: cameras[5]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView6
                    model: cameras[6]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView7
                    model: cameras[7]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView8
                    model: cameras[8]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                id: columnLayout1
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: 0.15 * parent.width

                Button {
                    text: qsTr("Capture Mode")
                    //id: button1
                    onClicked: {
                        for (var i = 0; i < cameras.length; ++i) {
                            cameras[i].state = Camera.CAMERA_CAPTURE
                        }
                    }
                }
                Button {
                    text: qsTr("Trigger All")
                    onClicked: {
                        capture.newCapture()
                        for (var i = 0; i < cameras.length; ++i) {
                            cameras[i].trigger()
                        }
                    }
                }
                FileDialog {
                    id: captureRootDialog
                    title: qsTr("Choose a location for the captured images")
                    folder: capture.captureRoot
                    selectFolder: true
                    onAccepted: {
                        // More or less hacky way to extract the absolute path from
                        // the URL that the FileDialog returns.
                        // see: http://stackoverflow.com/a/26868237/850264
                        var url = this.folder.toString();
                        // remove prefixed "file://"
                        var path = url.replace(/^(file:\/{2})/,"");
                        // unescape HTML codes like '%23'
                        path = decodeURIComponent(path);
                        capture.captureRoot = path;
                    }
                }
                RowLayout {
                    Text {
                        Layout.fillWidth: true
                        text: capture.captureRoot
                        elide: Text.ElideLeft
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
                Button {
                    text: qsTr("Save captured images")
                    onClicked: capture.saveCaptureToDisk()
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
                Button {
                    text: qsTr("Live Preview Mode")
                    //id: button1
                    onClicked: {
                        for (var i = 0; i < cameras.length; ++i) {
                            cameras[i].state = Camera.CAMERA_PREVIEW
                        }
                    }
                }
                Text {
                    text: capture.numCaptured + " / " + cameras.length
                }
            }
        }

        Tooltip {
            tool: autoSaveCheckbox
            text: "Saves the captured images as\nsoon as every camera triggered"
            opacity: autoSaveCheckbox.hovered ? 0.8 : 0
            // Hacky hack hack
            x: autoSaveCheckbox.mapToItem(mainFrame, 0 * columnLayout1.x * autoSaveCheckbox.x, 0).x + autoSaveCheckbox.width / 2 - width / 2
            y: autoSaveCheckbox.mapToItem(mainFrame, 0, 0 * columnLayout1.y * autoSaveCheckbox.y).y + autoSaveCheckbox.height + 5
            z: 9999
        }
    }

    function broadcastSettings(sourceCamera) {
        for (var i = 0; i < cameras.lenght; ++i) {
            var destCamera = cameras[i];
            if (sourceCamera !== destCamera) {
                destCamera.apertureIndex = sourceCamera.apertureIndex;
                destCamera.shutterIndex = sourceCamera.shutterIndex;
                destCamera.isoIndex = sourceCamera.isoIndex;
            }
        }
    }

    Connections {
        //target: liveImageProvider
        onStateChanged: {
            // refresh image grid here
        }
    }
}
