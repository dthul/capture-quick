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

    Row {
        visible: videoimporter.refreshing
        anchors.margins: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Text {
            id: infoText
            text: "Please stand by while the cameras' filesystems are being read"
        }
        BusySpinner {
            height: infoText.height
            running: true
        }
    }

    Column {
        visible: !videoimporter.refreshing
        anchors.margins: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Label {
            text: "There are at least " + videoimporter.minNumVideos + " and at most " + videoimporter.maxNumVideos + " clips on each camera."
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
                    enabled: !videoimporter.importRunning
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
            text: "Delete downloaded clips from camera"
            enabled: !videoimporter.importRunning
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
                enabled: !videoimporter.importRunning
                minimumValue: 1
                maximumValue: Math.max(videoimporter.maxNumVideos, 1)
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
        ProgressBar {
            visible: videoimporter.importRunning
            value: videoimporter.importProgress
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

}
