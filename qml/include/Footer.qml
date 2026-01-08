import QtQuick.Controls
import QtQuick 2.11
import components

ToolBar {
    id: toolBar
    height: 50

    Row {
        height: 50
        width: root.width

        SmallText {
            text: qsTr("Title")
        }
        SmallText {
            text: qsTr("Title")
        }
        SmallText {
            text: qsTr("Title")
        }
    }
}
