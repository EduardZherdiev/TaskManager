import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components

Dialog {
    id: dialog
    modal: true
    width: 400
    height: 300
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose
    padding: Style.mediumSpacing

    onOpened: {
        downloadNowButton.forceActiveFocus()
    }

    signal downloadRequested
    signal dismissRequested

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 2
        border.color: "#2196F3"
    }

    Column {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        // ===== ICON =====
        Rectangle {
            width: 60
            height: 60
            radius: 30
            color: "#2196F3"
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                anchors.centerIn: parent
                text: "⚡"
                font.pixelSize: 30
            }
        }

        // ===== TITLE =====
        Label {
            width: parent.width
            text: qsTr("Server Changes Detected")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        // ===== MESSAGE =====
        Column {
            width: parent.width
            spacing: Style.smallSpacing

            Rectangle {
                width: parent.width
                height: 70
                color: Style.componentBackground
                radius: Style.smallRadius

                Column {
                    anchors.fill: parent
                    anchors.margins: Style.mediumSpacing
                    spacing: Style.smallSpacing

                    Text {
                        text: qsTr("New tasks:") + " " + dialog.taskCount
                        color: Style.textColor
                        font.pixelSize: Style.smallFont
                    }

                    Text {
                        text: qsTr("New feedbacks:") + " " + dialog.feedbackCount
                        color: Style.textColor
                        font.pixelSize: Style.smallFont
                    }

                    Text {
                        text: qsTr("Last sync:") + " " + dialog.lastSyncTime
                        color: Style.secondaryText
                        font.pixelSize: Style.smallFont - 2
                    }
                }
            }
        }

        // ===== BUTTONS =====
        Row {
            width: parent.width
            spacing: Style.mediumSpacing
            layoutDirection: Qt.RightToLeft

            Button {
                id: downloadNowButton
                text: qsTr("Download Now")
                Layout.fillWidth: true
                onClicked: {
                    dialog.downloadRequested()
                    dialog.accept()
                }
            }

            Button {
                text: qsTr("Later")
                Layout.fillWidth: true
                type: 1
                onClicked: {
                    dialog.dismissRequested()
                    dialog.reject()
                }
            }
        }
    }

    property int taskCount: 0
    property int feedbackCount: 0
    property string lastSyncTime: ""

    function show(tasks, feedbacks, lastSync) {
        taskCount = tasks
        feedbackCount = feedbacks
        lastSyncTime = lastSync
        open()
    }
}
