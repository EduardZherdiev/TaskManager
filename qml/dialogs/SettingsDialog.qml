import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import components
import core
import dialogs 1.0

//Write simple Rectangle component





Dialog {
    id: dialog
    modal: true
    width: 400
    height: 550
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose

    signal profileEditRequested()
    signal signOutRequested()

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
    }

    Column {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        // ===== TITLE =====
        Label {
            width: parent.width
            text: qsTr("Settings")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        // ===== STORAGE =====
        Column {
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Storage")
                font.bold: true
                color: Style.textColor
            }

            Row {
                spacing: Style.mediumSpacing
                RadioButton {
                    text: qsTr("Save locally")
                    checked: AppSettings.storageMode === 0
                    onClicked: AppSettings.storageMode = 0
                }
                RadioButton {
                    text: qsTr("Save globally")
                    checked: AppSettings.storageMode === 1
                    onClicked: AppSettings.storageMode = 1
                }
            }
        }

        // ===== LANGUAGE =====
        Column {
            spacing: Style.smallSpacing
            width: parent.width

            Label {
                text: qsTr("Language")
                font.bold: true
                color: Style.textColor
            }

            ComboBox {
                width: parent.width
                model: ["English", "Українська", "Deutsch"]
                currentIndex: AppSettings.language
                onActivated: function(index) {
                    AppSettings.language = index
                }
            }
        }

        // ===== THEME =====
        Column {
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Theme")
                font.bold: true
                color: Style.textColor
            }

            Row {
                spacing: Style.mediumSpacing
                RadioButton {
                    text: qsTr("Light")
                    checked: !AppSettings.isDarkTheme
                    onClicked: {
                        AppSettings.isDarkTheme = false
                        Style.isDarkTheme = false
                    }
                }
                RadioButton {
                    text: qsTr("Dark")
                    checked: AppSettings.isDarkTheme
                    onClicked: {
                        AppSettings.isDarkTheme = true
                        Style.isDarkTheme = true
                    }
                }
            }
        }

        // ===== USER SECTION =====
        Rectangle {
            width: parent.width - 2 * Style.smallSpacing
            height: 1
            color: Style.componentOutline
        }

        Label {
            text: qsTr("User: ") + TaskModel.currentUserLogin
            font.bold: true
            color: Style.textColor
            font.pixelSize: Style.mediumFont
        }

            Button {
                text: qsTr("Profile")
                width: parent.width
                onClicked: {
                    dialog.profileEditRequested()
                }
            }

            Button {
                text: qsTr("Sign Out")
                width: parent.width
                type: 1
                onClicked: {
                    UserModel.signOut()
                    dialog.close()
                    dialog.signOutRequested()
                }
            }

            FeedbackDialog {
                id: feedbackDialog
            }

            Button {
                text: qsTr("Feedback")
                width: parent.width
                type: 1
                onClicked: {
                    feedbackDialog.open()
                }
            }

            Button {
                text: qsTr("Close")
                width: parent.width
                onClicked: dialog.close()
            }

    }
}
