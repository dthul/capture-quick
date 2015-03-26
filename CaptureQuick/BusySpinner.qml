import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

BusyIndicator {
    scale: 0.6
    style: BusyIndicatorStyle {
        indicator: Image {
            opacity: control.running ? 1 : 0
            source: "busyindicator.png"
            RotationAnimator on rotation {
                running: control.running
                loops: Animation.Infinite
                duration: 1000
                from: 0
                to: 360
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
        }
    }
}
