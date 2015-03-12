import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    width: 1920
    height: 1080

    property alias imageGrid: imageGrid

    RowLayout {
        id: rowLayout1
        anchors.fill: parent


        GridView {
            id: imageGrid
            model: imageGridModel
            cellWidth: width / 3
            cellHeight: height / 3
            Layout.fillWidth: true
            Layout.fillHeight: true
            delegate: Image {
                height: imageGrid.cellHeight
                width: imageGrid.cellWidth
                source: "image://live/" + url
                cache: false
                mipmap: true
                //fillMode: Image.PreserveAspectFit
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
        }
    }
}
