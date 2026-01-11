import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import components

Dialog {
    id: deleteDialog
    modal: true
    width: 400
    height: 150
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose

    property string taskTitle: ""
    property int taskId: -1

    signal confirmed(int taskId)

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.color: Style.componentOutline
        border.width: 1
    }

    Column {
        anchors.fill: parent
        anchors.margins: Style.smallSpacing
        spacing: Style.smallSpacing


        // ===== QUESTION =====
        Label {
            text: qsTr("Are you sure you want to delete this task?")
            font.pixelSize: Style.mediumFont
            color: Style.textColor
            wrapMode: Text.WordWrap
            width: parent.width
        }

        // ===== TASK TITLE =====
        Label {
            text: taskTitle
            font.pixelSize: Style.smallFont
            font.bold: true
            color: Style.textColor
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Item { height: Style.smallSpacing }

        // ===== BUTTONS =====
        Row {
            spacing: Style.mediumSpacing
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("Cancel")
                width: 120
                onClicked: deleteDialog.close()
            }

            Button {
                text: qsTr("Delete")
                width: 120
                type: 2
                onClicked: {
                    deleteDialog.confirmed(taskId)
                    deleteDialog.close()
                }
            }
        }
    }
}
