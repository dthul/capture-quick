import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import "qrc:/fontawesome.js" as FontAwesome

Window {
    modality: Qt.ApplicationModal

    title: "Download Videos from Cameras"

    width: 550
    height: 300
    minimumWidth: 400
    minimumHeight: 200

    onVisibleChanged: {
        if (visible) {
            // make sure that the cameras are connected
            capture.connect()
            // refresh the file list
            videoimporter.refresh()
        }
    }

    FileDialog {
        id: videoRootDialog
        title: qsTr("Choose a location for the captured videos")
        folder: videoimporter.videoRoot
        selectFolder: true
        onAccepted: {
            var path = extractPathFromURL(this.folder)
            videoimporter.videoRoot = path
        }
    }

    Column {
        anchors.margins: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Label {
            text: "There are at least " + videoimporter.minNumVideos + " clips on each camera."
        }
        Label {
            text: "Destination directory:"
        }
        RowLayout {
            Text {
                id: settings
                font.pointSize: 15
                font.family: "FontAwesome"
                text: FontAwesome.Icon.Cog
                MouseArea {
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    onClicked: videoRootDialog.open()
                }
            }
            Text {
                text: videoimporter.videoRoot
                elide: Text.ElideLeft
            }
        }
        CheckBox {
            text: "Delete clips from camera"
            checked: videoimporter.deleteFromCamera
            onCheckedChanged: {
                if (checked !== videoimporter.deleteFromCamera) {
                    videoimporter.deleteFromCamera = checked;
                }
            }
        }
        Row {
            spacing: 5
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "Download the latest"
            }
            SpinBox {
                id: numVideosSpinbox
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: 1
                maximumValue: Math.max(videoimporter.minNumVideos, 1)
                onValueChanged: videoimporter.numVideos = value
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "clips from each camera"
            }
        }
        Text {
            font.bold: true
            text: "Warning: there are not at least " + videoimporter.numVideos + " videos on each camera"
            visible: videoimporter.numVideos > videoimporter.minNumVideos
        }
        Row {
            Button {
                id: saveButton
                text: videoimporter.importRunning ? "Saving..." : "Save to disk"
                enabled: !videoimporter.importRunning
                onClicked: videoimporter.save()
            }
            BusySpinner {
                height: saveButton.height
                running: videoimporter.importRunning
            }
        }
    }

    Connections {
        target: videoimporter
        onNumVideosChanged: numVideosSpinbox.value = videoimporter.numVideos
    }

    onClosing: {
        if (videoimporter.importRunning) {
            close.accepted = false
        }
    }

    /*ColumnLayout {
        anchors.fill: parent

        ListView {
            id: importInfoView
            model: videoimporter.importInfos
            anchors.fill: parent
            delegate: ListView {
                id: fileNameView
                model: fileInfos
                anchors.fill: parent
                delegate: Text {
                    text: fileName + ": " + fileNameView.model.length + ": " + fileNameView.count + console.log(fileName)
                }
            }
        }
    }*/

    /*ColumnLayout {
        anchors.fill: parent
        ListView {
            anchors.fill: parent
            id: wtf1
            model: ListModel {
                ListElement {
                    fileInfos: [
                        ListElement { fileName: "1" },
                        ListElement { fileName: "2" },
                        ListElement { fileName: "3" }
                    ]
                }
            }

            delegate: ListView {
                anchors.fill: parent
                id: wtf2
                model: fileInfos
                delegate: Text {
                    text: fileName + console.log(fileName)
                }
            }
        }
    }*/

    /*ColumnLayout {
        id: layout1
        anchors.fill: parent
        ListView {
            anchors.fill: parent
            id: wtf1
            model: [[3, 4, 5, 6]]
            delegate: ListView {
                anchors.fill: parent
                id: wtf2
                model: wtf1.model[index]
                delegate: Text {
                    text: wtf2.model[index]
                }
            }
        }
    }*/

}
