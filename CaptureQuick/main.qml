import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    title: qsTr("Hello World")
    width: 1920
    height: 1080
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    Item {
        id: mainFrame
        anchors.fill: parent

        RowLayout {
            id: rowLayout1
            anchors.fill: parent

            GridLayout {
                id: imageGrid
                columns: 3
                rows: 3
                columnSpacing: 5
                rowSpacing: 5

                ImageView {
                    id: imageView0
                    model: imageGridModel[0]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView1
                    model: imageGridModel[1]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView2
                    model: imageGridModel[2]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView3
                    model: imageGridModel[3]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView4
                    model: imageGridModel[4]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView5
                    model: imageGridModel[5]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView6
                    model: imageGridModel[6]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView7
                    model: imageGridModel[7]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ImageView {
                    id: imageView8
                    model: imageGridModel[8]

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                id: columnLayout1
                width: 100
                height: 100
                Layout.alignment: Qt.AlignTop

                ComboBox {
                    id: comboBox1
                }

                Button {
                    text: qsTr("Press me")
                    id: button1
                }
            }
        }
    }

    Connections {
        //target: liveImageProvider
        onStateChanged: {
            // refresh image grid here
        }
    }
}
