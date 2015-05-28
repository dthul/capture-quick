import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

Window {
    modality: Qt.ApplicationModal

    title: "Capture Videos"

    width: 550
    height: 300
    minimumWidth: 400
    minimumHeight: 200

    property string status: "connected" // "disconnected", "disconnecting", "connecting"
    property string step: "explanationbefore" // "capture", "explanationafter"
    property bool captureActive: false
    property date now: new Date()
    property date started: new Date()

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

    Timer {
        interval: 100
        repeat: true
        running: true
        onTriggered: now = new Date()
    }

    function formatTimeDifference(diff) {
        var seconds = Math.round(diff / 1000);
        var minutes = Math.floor(seconds / 60);
        seconds = seconds - 60 * minutes;
        return minutes + ":" + (seconds < 10 ? "0" + seconds : seconds);
    }

    Label {
        visible: step == "explanationbefore" && status == "disconnecting"
        text: "Waiting for all cameras to be released, please stand by..."
    }

    /*Rectangle {
        x: wtf.x + 2
        y: wtf.y + 2
        width: wtf.width - 4
        height: wtf.height - 4
        color: "red"
    }*/

    Item {
        anchors.fill: parent

        ColumnLayout {
            anchors.margins: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: step == "explanationbefore" && status == "disconnected"
            Text {
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                text: "To start capturing videos, follow these steps:<br>
                        <ul>
                            <li>disconnect the cameras from USB</li>
                            <li>switch them to OFF</li>
                            <li>switch them to video mode</li>
                        </ul><br>
                        All cameras' screens should now show the normal live preview with the video option overlays.<br>
                        <ul>
                            <li>make sure that the trigger box is connected via USB</li>
                        </ul>"
            }
            Button {
                text: "Done!"
                onClicked: step = "capture"
            }
        }

        ColumnLayout {
            anchors.margins: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            visible: step == "capture" && status == "disconnected"
            Text {
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                text: "Use the button below to start and stop video recording.
                        You can record multiple videos after one another.<br>
                        The cameras' red back LEDs should blink during recording."
            }
            Button {
                text: captureActive ?
                          "Stop Recording (" + formatTimeDifference(Math.max(0, now - started)) + ")"
                        :
                          "Start Recording"
                iconSource: captureActive ? "qrc:/stop.png" : "qrc:/record.png"
                onClicked: {
                    capture.halfPressShutterAll()
                    started = new Date()
                    captureActive = !captureActive
                }
            }
            Button {
                text: "End video capture"
                enabled: !captureActive
                onClicked: step = "explanationafter"
            }
        }

        ColumnLayout {
            anchors.margins: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: step == "explanationafter" && status == "disconnected"
            Text {
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                text: "Undo your previous changes now. In detail this means:<br>
                        <ul>
                            <li>switch all cameras back to photo mode</li>
                            <li>connect them to USB again and wait until the OS has recognized them</li>
                            <li>make sure that the trigger box is connected to USB</li>
                        </ul>"
            }
            Button {
                text: "Done"
                onClicked: close()
            }
        }
    }

}
