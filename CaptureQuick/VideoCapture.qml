import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3

Window {
    modality: Qt.ApplicationModal

    property string status: "connected" // "disconnected", "disconnecting", "connecting"
    property string step: "explanationbefore" // "capture", "explanationafter"
    property bool captureActive: false

    onVisibleChanged: {
        if (visible) {
            step = "explanationbefore"
            status = "disconnecting"
            capture.disconnect()
            status = "disconnected"
        }
    }

    onClosing: {
        if (captureActive || step === "capture") {
            close.accepted = false
            return
        }
        status = "connecting"
        capture.connect()
        status = "connected"
    }

    Label {
        visible: step == "explanationbefore" && status == "disconnecting"
        text: "Waiting for all cameras to be released, please stand by..."
    }

    Column {
        visible: step == "explanationbefore" && status == "disconnected"
        Text {
            textFormat: Text.StyledText
            text: "Execute these steps for all cameras:<br>
                    <ul>
                        <li>disconnect the camera from USB</li>
                        <li>switch it to OFF</li>
                        <li>switch it to video mode</li>
                    </ul><br>
                    All cameras' screens should show the normal live preview with the video option overlays now.<br>
                    <ul>
                        <li>make sure that the trigger box is connected via USB</li>
                    </ul>"
        }
        Button {
            text: "Done!"
            onClicked: step = "capture"
        }
    }

    Column {
        visible: step == "capture" && status == "disconnected"
        Button {
            text: captureActive ? "Stop Recording" : "Start Recording"
            onClicked: {
                capture.halfPressShutterAll()
                captureActive = !captureActive
            }
        }
        Button {
            text: "End video capture"
            enabled: !captureActive
            onClicked: step = "explanationafter"
        }
    }

    Column {
        visible: step == "explanationafter" && status == "disconnected"
        Label {
            text: "Switch all cameras back to photo mode, connect them to USB again and wait until the OS has recognized them"
        }
        Button {
            text: "Done"
            onClicked: close()
        }
    }

}
