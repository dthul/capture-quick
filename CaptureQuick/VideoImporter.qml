import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

Window {
    modality: Qt.ApplicationModal

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

    Column {
        Label {
            text: "There are at least " + videoimporter.minNumVideos + " on each camera."
        }
        Row {
            Label {
                text: "Download the latest"
            }
            SpinBox {
                id: numVideosSpinbox
                minimumValue: 1
                maximumValue: Math.max(videoimporter.minNumVideos, 1)
                onValueChanged: videoimporter.numVideos = value
            }
            Label {
                text: "clips from each camera"
            }
        }
        Label {
            text: "Warning: there are not at least " + videoimporter.numVideos + " videos on each camera"
            visible: videoimporter.numVideos > videoimporter.minNumVideos
        }
        Row {
            Button {
                text: videoimporter.importRunning ? "Saving..." : "Save to disk"
                enabled: !videoimporter.importRunning
                onClicked: videoimporter.save()
            }
            BusySpinner {
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
