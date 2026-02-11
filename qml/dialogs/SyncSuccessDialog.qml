import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Dialog {
    id: root
    modal: true
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    title: ""
    width: 420
    height: 220

    property string dialogTitle: ""
    property string message: ""

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor

        Rectangle {
            visible: Style.isDarkTheme
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Style.componentOutline
        }

        Rectangle {
            visible: Style.isDarkTheme
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Style.componentOutline
        }
    }

    onOpened: okButton.forceActiveFocus()

    header: Rectangle {
        height: 42
        color: Style.surfaceColor
        border.color: Style.componentOutline
        border.width: Style.isDarkTheme ? 0 : 1

        Rectangle {
            visible: Style.isDarkTheme
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: Style.componentOutline
        }

        Rectangle {
            visible: Style.isDarkTheme
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Style.componentOutline
        }

        Rectangle {
            visible: Style.isDarkTheme
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Style.componentOutline
        }

        Label {
            anchors.centerIn: parent
            text: root.dialogTitle
            color: Style.textColor
            font.bold: true
        }
    }

    contentItem: TextEdit {
        color: Style.textColor
        readOnly: true
        text: root.message
        wrapMode: TextEdit.Wrap
        padding: 12
        selectedTextColor: Style.textColor
        selectionColor: Style.primaryColor
        selectByMouse: true
    }

    footer: Item {
        width: parent.width
        height: 54

        Rectangle {
            anchors.fill: parent
            color: Style.surfaceColor
            border.color: Style.componentOutline
            border.width: Style.isDarkTheme ? 0 : 1

            Rectangle {
                visible: Style.isDarkTheme
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: Style.componentOutline
            }

            Rectangle {
                visible: Style.isDarkTheme
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }

            Rectangle {
                visible: Style.isDarkTheme
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }
        }

        Button {
            id: okButton
            text: qsTr("OK")
            anchors.centerIn: parent
            onClicked: root.close()
            Keys.onReturnPressed: root.close()
            Keys.onEscapePressed: root.close()
        }
    }
}
