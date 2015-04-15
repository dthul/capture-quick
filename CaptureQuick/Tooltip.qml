import QtQuick 2.4

Rectangle {
    property string text
    color: "black"
    width: text.width + 20
    height: text.height + 10
    radius: 5

    Text {
        id: text
        horizontalAlignment: Text.AlignHCenter
        color: Qt.rgba(1,1,1,1)
        text: parent.text
        anchors.centerIn: parent
    }
}
