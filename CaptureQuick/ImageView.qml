import QtQuick 2.4
import QtQuick.Layouts 1.1

Item {
    id: imageView

    property QtObject model: null

    states: State {
        name: "maximized"
        PropertyChanges {
            target: overlay
            x: imageGrid.x
            y: imageGrid.y
            z: 2
            width: imageGrid.width
            height: imageGrid.height
            opacity: 1
            enabled: true
        }
        PropertyChanges {
            target: info
            z: 3
        }
    }

    transitions: [
        Transition {
            from: ""
            to: "maximized"
            SequentialAnimation {
                NumberAnimation {
                    target: info
                    property: "z"
                    duration: 0
                }
                NumberAnimation {
                    target: overlay
                    properties: "z,opacity"
                    duration: 0
                }
                NumberAnimation {
                    target: overlay
                    properties: "x,y,width,height"
                    duration: 200
                    easing.type: Easing.InOutCubic
                }
            }
        },
        Transition {
            from: "maximized"
            to: ""
            SequentialAnimation {
                NumberAnimation {
                    target: overlay
                    properties: "x,y,width,height"
                    duration: 200
                    easing.type: Easing.InOutCubic
                }
                NumberAnimation {
                    target: overlay
                    properties: "z,opacity"
                    duration: 0
                }
                NumberAnimation {
                    target: info
                    property: "z"
                    duration: 0
                }
            }
        }
    ]

    MouseArea {
        id: overlay

        // Make imageGrid the overlay's parent so that it is effectively taken out
        // of the GridLayout
        parent: mainFrame

        // Map the image's coordinates (which are relative to its respective GridLayout cell)
        // to the upper left corner of the imageGrid (which is the overlay's parent) to position
        // the overlay exactly over the image.
        // The multiplication of the image's and imageGrid's properties with 0 is a hack to
        // enforce recomputation of the (mapped) x and y coordinates when either the image
        // or the imageGrid change size or position
        x: image.mapToItem(imageGrid, 0 * image.width * image.x * imageGrid.x * imageGrid.width, 0).x
        y: image.mapToItem(imageGrid, 0 * image.height * image.y * imageGrid.y * imageGrid.height, 0).y

        // Always keep the same size as the image
        width: image.width
        height: image.height

        // Ignore all mouse events when not in full state.
        // Otherwise the overlay would intercept all mouse events since it is on top
        // of the other items
        enabled: false

        // Hide the overlay initially
        opacity: 0

        // Close the overlay on click
        onClicked: imageView.state = ""

        z: 0

        // This is the overlayed image that the user will actually see
        // when opening the overlay
        Image {
            id: overlayImage
            width: overlay.width
            height: overlay.height
            clip: true
            source: image.source
            mipmap: true
        }
    }

    MouseArea {
        id: imageArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: state = "hovered"
        onExited: state = ""

        states: State {
            name: "hovered"
            PropertyChanges {
                target: glow
                opacity: 0.1
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

        // This is the image that is actually visible in
        // the image grid.
        Image {
            id: image
            cache: false
            mipmap: true
            width: parent.width
            height: parent.height
            // The "live" image provider has been registered from
            // the C++ code
            source: "image://live/" + model.url
        }

        // This rectangle will provide the white "glow" when
        // hovering with the mouse over an image
        Rectangle {
            id: glow
            // Position the rectangle exactly over the image
            x: image.x
            y: image.y
            width: image.width
            height: image.height
            color: "white"
            // Hide the glow initially
            opacity: 0
        }

        // Open the overlay on click
        onClicked: imageView.state = "maximized"
    }

    Item {
        id: info

        states: State {
            name: "visible"
            PropertyChanges {
                target: infoRect
                opacity: 0.7
            }
            PropertyChanges {
                target: infoImg
                rotation: 0
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "visible"
                NumberAnimation {
                    target: infoRect
                    property: "opacity"
                    duration: 100
                }
                NumberAnimation {
                    target: infoImg
                    property: "rotation"
                    duration: 100
                }
            },
            Transition {
                from: "visible"
                to: ""
                NumberAnimation {
                    target: infoRect
                    property: "opacity"
                    duration: 100
                }
                NumberAnimation {
                    target: infoImg
                    property: "rotation"
                    duration: 100
                }
            }
        ]

        parent: mainFrame
        anchors.right: overlay.right
        anchors.rightMargin: 5
        anchors.top: overlay.top
        anchors.topMargin: 5
        width: infoPanel.width + 20
        height: infoPanel.height + 20
        z: 1

        Rectangle {
            id: infoRect
            color: "black"
            radius: 10
            opacity: 0
            anchors.fill: parent
            ColumnLayout {
                id: infoPanel
                anchors.centerIn: parent
                Text {
                    color: "white"
                    text: "Info"
                }
                Text {
                    color: "white"
                    text: "more info"
                }
            }
        }

        MouseArea {
            anchors.right: info.right
            anchors.top: info.top
            anchors.topMargin: 5
            anchors.rightMargin: 5
            width: infoImg.width
            height: infoImg.height
            Image {
                id: infoImg
                source: "qrc:/close.svg"
                width: 15
                height: 15
                rotation: -45
            }

            onClicked: info.state = info.state == "" ? "visible" : ""
        }
    }

}
