import QtQuick 2.4
import QtQuick.Controls 1.3

MouseArea {
    id: tag
    property QtObject image
    property string text
    width: rect.width
    height: image.empty ? 0 : rect.height
    enabled: !image.empty
    opacity: image.empty ? 0 : 1
    cursorShape: image.saved ? Qt.PointingHandCursor : Qt.ArrowCursor
    Rectangle {
        id: rect
        width: label.width + 7
        height: label.height + 3
        radius: 2
        // color hue: 232°
        color: image.saved ? "#143054" : "#2C2C2C"

        Label {
            id: label
            anchors.centerIn: parent
            text: tag.text
            color: image.saved ? "#4C688B" : "#646464"
        }
    }
    onClicked: image.show()
}
