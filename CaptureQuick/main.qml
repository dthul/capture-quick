import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
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
        objectName: "mainFrame"
        anchors.fill: parent

        property alias imageGrid: imageGrid

        MouseArea {
            id: overlay
            x: 0
            y: 0
            width: 12
            height: 12
            z: 999
            property alias image: overlayImage
            onClicked: {
                overlay.state = ""
            }
            //color: "red";

            property int start_x: 0
            property int start_y: 0
            property int start_width: 0
            property int start_height: 0

            Image {
                id: overlayImage
                width: parent.width
                height: parent.height
                //color: "red"
                clip: true
            }
            states: [
                State {
                    name: "full"
                    //when: button1.pressed == true
                    PropertyChanges {
                        target: overlay
                        x: imageGrid.x
                        y: imageGrid.y
                        width: imageGrid.width
                        height: imageGrid.height
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""
                    to: "full"
                    SequentialAnimation {
                        ParallelAnimation {
                            NumberAnimation {
                                target: overlay
                                property: "x"
                                to: overlay.start_x
                                duration: 0
                            }
                            NumberAnimation {
                                target: overlay
                                property: "y"
                                to: overlay.start_y
                                duration: 0
                            }
                            NumberAnimation {
                                target: overlay
                                property: "width"
                                to: overlay.start_width
                                duration: 0
                            }
                            NumberAnimation {
                                target: overlay
                                property: "height"
                                to: overlay.start_height
                                duration: 0
                            }
                        }

                        NumberAnimation {
                            target: overlay
                            properties: "x,y,width,height"
                            duration: 200
                        }
                    }
                },
                Transition {
                    from: "full"
                    to: ""
                    NumberAnimation {
                        target: overlay
                        property: "x"
                        to: overlay.start_x
                        duration: 200
                    }
                    NumberAnimation {
                        target: overlay
                        property: "y"
                        to: overlay.start_y
                        duration: 200
                    }
                    NumberAnimation {
                        target: overlay
                        property: "width"
                        to: overlay.start_width
                        duration: 200
                    }
                    NumberAnimation {
                        target: overlay
                        property: "height"
                        to: overlay.start_height
                        duration: 200
                    }
                }
            ]
        }

        RowLayout {
            id: rowLayout1
            objectName: "rowLayout1"
            anchors.fill: parent

            GridLayout {
                id: imageGrid
                objectName: "imageGrid"
                columns: 3
                rows: 3
                columnSpacing: 5
                rowSpacing: 5
                MouseArea {
                    id: mouse0
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Image {
                        id: image0
                        cache: false
                        mipmap: true
                        width: parent.width
                        height: parent.height
                        source: "image://live/" + imageGridModel[0].url
                        /*states: State {
                            name: "big"
                            when: button1.pressed == true
                            ParentChange {
                                target: image0
                                parent: mainFrame
                            }
                        }
                        transitions: Transition {
                            from: ""
                            to: "big"
                            reversible: true

                            ParentAnimation {
                            }
                        }*/
                    }

                    onClicked: {
                        overlay.x = image0.x
                        overlay.y = image0.y
                        overlay.width = image0.width
                        overlay.height = image0.height
                        overlay.opacity = 1
                        overlay.image.source = image0.source
                        overlay.state = "full"
                    }
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 0
                    Layout.column: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[1].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 0
                    Layout.column: 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[2].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 1
                    Layout.column: 0
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[3].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 1
                    Layout.column: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[4].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 1
                    Layout.column: 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[5].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 2
                    Layout.column: 0
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[6].url
                }
                Image {
                    cache: false
                    mipmap: true
                    Layout.row: 2
                    Layout.column: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: "image://live/" + imageGridModel[7].url
                }
                Text {
                    text: overlay.state
                }
            }

            /*GridView {
                id: imageGrid
                model: imageGridModel
                cellWidth: width / 3
                cellHeight: height / 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                delegate: Image {
                    id: delegate_image
                    height: imageGrid.cellHeight
                    width: imageGrid.cellWidth
                    source: "image://live/" + url
                    cache: false
                    mipmap: true
                    //fillMode: Image.PreserveAspectFit
                    states: State {
                        name: "big"
                        when: button1.pressed == true
                        ParentChange {
                            //target: delegate_image
                            parent: imageGrid
                        }
                    }
                    transitions: Transition {
                        from: ""
                        to: "big"
                        reversible: true

                        ParentAnimation {


                        }
                    }
                }
            }*/

            ColumnLayout {
                id: columnLayout1
                width: 100
                height: 100
                Layout.alignment: Qt.AlignTop

                ComboBox {
                    id: comboBox1
                }

                Button {
                    text: "Press me"
                    id: button1
                    onClicked: {
                        //overlay.state = "image0"
                        overlay.state = "full"
                    }
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
