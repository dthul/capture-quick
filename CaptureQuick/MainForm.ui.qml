import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    width: 640
    height: 480

    property alias button3: button3
    property alias button2: button2
    property alias button1: button1
    property alias imageGrid: imageGrid

    RowLayout {
        anchors.centerIn: parent

        Button {
            id: button1
            text: qsTr("Press Me 1")
        }

        Button {
            id: button2
            text: qsTr("Press Me 2")
        }

        Button {
            id: button3
            text: qsTr("Press Me 3")
        }

        GridView {
            id: imageGrid
            model: imageGridModel
            x: 0
            y: 0
            width: 140
            height: 140
            cellWidth: width / 3
            delegate: Image {
                source: "image://live/" + url
            }
        }
    }
}
