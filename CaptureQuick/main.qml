import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import CaptureQuick 0.1 // Makes the Camera class available (registered in main.cpp)

import "qrc:/fontawesome.js" as FontAwesome

ApplicationWindow {
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
                width: 100
                height: 100
                Layout.alignment: Qt.AlignTop

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
                        capture.captureRoot = captureRootDialog.folder
                    }
                }
                RowLayout {
                    Text {
                        text: capture.captureRoot
                        elide: Text.ElideMiddle
                    }
                    Text {
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
    }

    Connections {
        //target: liveImageProvider
        onStateChanged: {
            // refresh image grid here
        }
    }
}
