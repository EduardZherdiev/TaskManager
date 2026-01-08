import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import components

//Write simple Rectangle component





Dialog {
    id: dialog
    modal: true
    width: 400
    height: 480
    parent: Overlay.overlay

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
    }

    Column {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        // ===== TITLE =====
        Label {
            text: qsTr("Settings")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
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
                RadioButton { text: qsTr("Save locally"); checked: true }
                RadioButton { text: qsTr("Save globally") }
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
                    checked: !Style.isDarkTheme
                    onClicked: Style.isDarkTheme = false
                }
                RadioButton {
                    text: qsTr("Dark")
                    checked: Style.isDarkTheme
                    onClicked: Style.isDarkTheme = true
                }
            }
        }



            Button {
                text: qsTr("Profile")
                width: parent.width
            }

            Button {
                text: qsTr("Tutorial")
                width: parent.width
            }

            Button {
                text: qsTr("Feedback")
                width: parent.width
                type: 1
            }

            Button {
                text: qsTr("Close")
                width: parent.width
                onClicked: dialog.close()
            }

    }
}
