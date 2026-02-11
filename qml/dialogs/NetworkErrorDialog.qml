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
    height: 250

    property string message: ""
    property bool sessionExpired: false

    signal signInRequested()

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

    onOpened: {
        errorText.focus = false
        actionButton.forceActiveFocus()
    }

    contentItem: TextEdit {
        id: errorText
        color: Style.textColor
        readOnly: true
        text: root.message
        wrapMode: TextEdit.Wrap
        padding: 10
        selectedTextColor: Style.textColor
        selectionColor: Style.primaryColor
        selectByMouse: true
    }

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
            text: root.sessionExpired ? qsTr("Session expired") : qsTr("Server error")
            color: Style.textColor
            font.bold: true
        }
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

        Row {
            spacing: Style.mediumSpacing
            anchors.centerIn: parent

            Button {
                id: cancelSessionButton
                visible: root.sessionExpired
                width: visible ? implicitWidth : 0
                text: qsTr("Cancel")
                onClicked: root.close()
            }

            Button {
                id: actionButton
                text: root.sessionExpired ? qsTr("Sign in") : qsTr("OK")

                onClicked: {
                    root.close()
                    if (root.sessionExpired) {
                        root.signInRequested()
                    }
                }

                Keys.onReturnPressed: {
                    root.close()
                    if (root.sessionExpired) {
                        root.signInRequested()
                    }
                }
                Keys.onEscapePressed: {
                    root.close()
                }
            }
        }
    }
}
