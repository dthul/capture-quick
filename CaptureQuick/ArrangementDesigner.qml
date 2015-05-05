import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3

import "qrc:/fontawesome.js" as FontAwesome

Window {
    width: 1920
    height: 1080

    Rectangle {
        anchors.fill: parent
        color: "lightblue"
    }
    Column {
        id: step1
        Row {
            SpinBox {
                id: heightSpinner
                minimumValue: 1
                maximumValue: 8
                value: 3
            }
            Label {
                font.pointSize: 15
                font.family: "FontAwesome"
                text: FontAwesome.Icon.Times
            }
            SpinBox {
                id: widthSpinner
                minimumValue: 1
                maximumValue: 8
                value: 3
            }
        }
    }
}

