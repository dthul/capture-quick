import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import CaptureQuick 0.1 // Makes the Camera class available (registered in main.cpp)

Item {
    id: imageView

    property QtObject model: capture.uiCameras[index]

    states: State {
        name: "maximized"
        PropertyChanges {
            target: overlay
            x: imageGrid.mapToItem(mainFrame, 0 * imageGrid.x * imageGrid.width, 0).x
            y: imageGrid.mapToItem(mainFrame, 0 * imageGrid.x * imageGrid.width, 0).y
            z: 2
            width: imageGrid.width
            height: imageGrid.height
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
                    properties: "z"
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
                    properties: "z"
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

        // Make mainFrame the overlay's parent so that it is effectively taken out
        // of the GridLayout
        parent: mainFrame

        hoverEnabled: true

        // Map the image's coordinates (which are relative to its respective GridLayout cell)
        // to the upper left corner of the mainFrame (which is the overlay's parent) to position
        // the overlay exactly over the image.
        // The multiplication of the image's and imageGrid's properties with 0 is a hack to
        // enforce recomputation of the (mapped) x and y coordinates when either the image
        // or the imageGrid change size or position
        x: image.mapToItem(mainFrame, 0 * image.x * imageView.x * imageLoader.x * imageView.width * imageGrid.x, 0).x
        y: image.mapToItem(mainFrame, 0 * image.y * imageView.y * imageLoader.y * imageView.height * imageGrid.y, 0).y

        // Always keep the same size as the image
        width: image.width
        height: image.height

        // Close the overlay on click
        onClicked: imageView.state = imageView.state == "" ? "maximized" : ""

        z: 0

        state: containsMouse ? "hovered" : ""

        states: State {
            name: "hovered"
            PropertyChanges {
                target: glow
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

        RectangularGlow {
            id: glow
            anchors.fill: parent
            color: "orange"
            cornerRadius: 0
            glowRadius: 5
            opacity: 0
        }

        Rectangle {
            anchors.fill: parent
            color: 'grey'
        }

        // This is the overlayed image that the user will actually see
        Image {
            id: overlayImage
            // Works only for multiples of 90°
            width: rotation % 180 == 0 ? overlay.width : overlay.height
            height: rotation % 180 == 0 ? overlay.height : overlay.width
            // anchors.fill: parent
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            clip: true
            source: image.source
            mipmap: true
            rotation: capture.uiCameraRotations[index]
        }

        MouseArea {
            id: jpegTag
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            width: jpegRect.width
            height: jpegRect.height
            enabled: !model.image.empty
            opacity: model.image.empty ? 0 : 1
            cursorShape: model.image.saved ? Qt.PointingHandCursor : Qt.ArrowCursor
            Rectangle {
                id: jpegRect
                width: jpegLabel.width + 7
                height: jpegLabel.height + 3
                radius: 2
                color: "white"
                Label {
                    id: jpegLabel
                    anchors.centerIn: parent
                    text: "JPEG"
                    color: model.image.saved ? "blue" : "gray"
                }
            }
            onClicked: model.image.show()
        }
    }

    // This is the image that is actually in
    // the image grid.
    Image {
        id: image
        //anchors.fill: parent
        width: parent.width - 5
        height: parent.height - 5
        anchors.centerIn: parent
        // The "live" image provider has been registered from
        // the C++ code
        source: "image://live/" + ((model.state === Camera.CAMERA_PREVIEW) ? model.preview.url : ((!model.image.empty) ? model.image.url : model.rawImage.url))
        visible: false
    }

    Item {
        id: info
        // state: "visible"
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
            enabled: info.state == "visible"
            ColumnLayout {
                id: infoPanel
                anchors.centerIn: parent
                Text {
                    textFormat: Text.StyledText
                    color: "white"
                    text: "<b>Name:</b> " + model.name
                    font.pointSize: 12
                }
                Text {
                    property int state: model.state
                    property string stateName: "Unknown"
                    onStateChanged: {
                        switch(state) {
                        case Camera.CAMERA_INIT: stateName = "Initializing"; break
                        case Camera.CAMERA_CAPTURE: stateName = "Capture"; break
                        case Camera.CAMERA_NONE: stateName = "Unconnected"; break
                        case Camera.CAMERA_PREVIEW: stateName = "Live Preview"; break
                        case Camera.CAMERA_SHUTDOWN: stateName = "Disconnecting"; break
                        case Camera.CAMERA_TRANSITIONING: stateName = "Transitioning"; break
                        }
                    }

                    textFormat: Text.StyledText
                    color: "white"
                    text: "<b>State:</b> " + stateName
                    font.pointSize: 12
                }
                Text {
                    textFormat: Text.StyledText
                    color: "white"
                    text: "<b>Aperture:</b> " + model.aperture
                    font.pointSize: 12
                }
                RowLayout {
                    spacing: 0
                    ComboBox {
                        id: apertureComboBox
                        model: imageView.model.apertureChoices
                        property int selectedIndex: imageView.model.apertureIndex
                        property bool synced: false
                        onSelectedIndexChanged: {
                            if (currentIndex != selectedIndex)
                                currentIndex = selectedIndex
                            synced = true
                        }
                        onCurrentIndexChanged: {
                            if (currentIndex != selectedIndex) {
                                imageView.model.apertureIndex = currentIndex
                                synced = false
                            }
                            else {
                                synced = true
                            }
                        }

                        // The following intuitive property binding does not work so the
                        // onSelectedIndexChanged and onCurrentIndexChanged listeners
                        // are used instead.
                        // currentIndex: imageView.model.apertureIndex
                    }
                    BusySpinner {
                        Layout.maximumHeight: apertureComboBox.height
                        Layout.maximumWidth: apertureComboBox.height
                        running: !apertureComboBox.synced
                    }
                }
                Text {
                    textFormat: Text.StyledText
                    color: "white"
                    text: "<b>Shutter Speed:</b> " + model.shutter
                    font.pointSize: 12
                }
                RowLayout {
                    spacing: 0
                    ComboBox {
                        id: shutterComboBox
                        model: imageView.model.shutterChoices
                        property int selectedIndex: imageView.model.shutterIndex
                        property bool synced: false
                        onSelectedIndexChanged: {
                            if (currentIndex != selectedIndex)
                                currentIndex = selectedIndex
                            synced = true
                        }
                        onCurrentIndexChanged: {
                            if (currentIndex != selectedIndex) {
                                imageView.model.shutterIndex = currentIndex
                                synced = false
                            }
                            else {
                                synced = true
                            }
                        }
                    }
                    BusySpinner {
                        Layout.maximumHeight: shutterComboBox.height
                        Layout.maximumWidth: shutterComboBox.height
                        running: !shutterComboBox.synced
                    }
                }
                Text {
                    textFormat: Text.StyledText
                    color: "white"
                    text: "<b>ISO:</b> " + model.iso
                    font.pointSize: 12
                }
                RowLayout {
                    spacing: 0
                    ComboBox {
                        id: isoComboBox
                        model: imageView.model.isoChoices
                        property int selectedIndex: imageView.model.isoIndex
                        property bool synced: false
                        onSelectedIndexChanged: {
                            if (currentIndex != selectedIndex)
                                currentIndex = selectedIndex
                            synced = true
                        }
                        onCurrentIndexChanged: {
                            if (currentIndex != selectedIndex) {
                                imageView.model.isoIndex = currentIndex
                                synced = false
                            }
                            else {
                                synced = true
                            }
                        }
                    }
                    BusySpinner {
                        Layout.maximumHeight: isoComboBox.height
                        Layout.maximumWidth: isoComboBox.height
                        running: !isoComboBox.synced
                    }
                }
                Button {
                    text: "Trigger"
                    onClicked: model.trigger()
                }
                Button {
                    text: "Broadcast settings"
                    onClicked: app.broadcastSettings(imageView.model)
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

    Component.onDestruction: {
        // reparent the elements from the mainFrame to their
        // initial component so that they get properly destroyed
        overlay.parent = imageView
        info.parent = imageView
    }
}
