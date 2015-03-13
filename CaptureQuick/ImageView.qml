import QtQuick 2.4

Item {
    id: imageView

    property QtObject model: null

    states: [
        State {
            name: "maximized"
            PropertyChanges {
                target: overlay
                x: imageGrid.x
                y: imageGrid.y
                width: imageGrid.width
                height: imageGrid.height
                opacity: 1
                enabled: true
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "maximized"
            SequentialAnimation {
                NumberAnimation {
                    target: overlay
                    property: "opacity"
                    duration: 0
                }
                NumberAnimation {
                    target: overlay
                    properties: "x,y,width,height"
                    duration: 200
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
                }
                NumberAnimation {
                    target: overlay
                    property: "opacity"
                    duration: 0
                }
            }
        }
    ]

    MouseArea {
        id: overlay

        // Make imageGrid the overlay's parent so that it is effectively taken out
        // of the GridLayout
        parent: imageGrid

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
        onClicked: {
            imageView.state = ""
        }

        // This is the overlayed image that the user will actually see
        // when opening the overlay
        Image {
            id: overlayImage
            width: parent.width
            height: parent.height
            clip: true
            source: image.source
            z: 999 // needed?
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
                target: shine
                opacity: 0.1
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "hovered"
                NumberAnimation {
                    target: shine
                    property: "opacity"
                    duration: 100
                }
            },
            Transition {
                from: "hovered"
                to: ""
                NumberAnimation {
                    target: shine
                    property: "opacity"
                    duration: 100
                }
            }
        ]

        Image {
            id: image
            cache: false
            mipmap: true
            width: parent.width
            height: parent.height
            source: "image://live/" + model.url
        }

        // This rectangle will provide the white "glow" when
        // moving the mouse over an image
        Rectangle {
            id: shine
            x: image.x
            y: image.y
            width: image.width
            height: image.height
            color: "white"
            opacity: 0
        }

        onClicked: {
            imageView.state = "maximized"
        }
    }
}
