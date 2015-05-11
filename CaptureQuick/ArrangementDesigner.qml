import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3

import "qrc:/fontawesome.js" as FontAwesome

Window {
    width: 1920
    height: 1080

    property int step: 1

    Item {
        id: designer
        anchors.fill: parent
        state: "step1"

        states: State {
            name: "step1"
            PropertyChanges {
                target: step1
                opacity: 1
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "hovered"
                NumberAnimation {
                    target: glow
                    property: "opacity"
                    duration: 100
                }
            },
            Transition {
                from: "hovered"
                to: ""
                NumberAnimation {
                    target: glow
                    property: "opacity"
                    duration: 100
                }
            }
        ]

        Rectangle {
            anchors.fill: parent
            color: "lightblue"
        }
        Column {
            id: step1
            opacity: 0
            anchors.centerIn: parent
            Row {
                id: sizeChooser
                anchors.horizontalCenter: parent.horizontalCenter
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
                    anchors.verticalCenter: parent.verticalCenter
                }
                SpinBox {
                    id: widthSpinner
                    minimumValue: 1
                    maximumValue: 8
                    value: 3
                }
            }
            Rectangle {
                width: 1
                height: 10
                color: "transparent"
            }
            Button {
                text: "Next"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: step = 2
            }
        }

    }

    onStepChanged: {

    }
}

