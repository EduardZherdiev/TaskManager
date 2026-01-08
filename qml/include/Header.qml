import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import components
import QtQuick.Effects
import core
import dialogs

Rectangle {
    width: parent.width
    height: 86
    color: Style.surfaceColor

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Style.controlBorder
    }

    Row {
        anchors.centerIn: parent
        spacing: Style.bigSpacing

        // ===== SORTING =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Row {
                id: sortRow
                spacing: Style.smallSpacing
                property bool sort: true // desk
                property string sortImg: sort ? "arrow-down" : "arrow-up"

                Column {
                    spacing: 2

                    Label {
                        leftPadding: 7
                        text: qsTr("Sorting")
                        color: Style.textColor
                    }
                    ComboBox {
                        id: sortBy
                        model: ["Created at", "Title", "State", "Updated at", "Deleted at"]
                    }
                }
                ImgButton {
                    width: sortBy.height
                    height: sortBy.height
                    imgScale: 1.3
                    anchors.bottom: parent.bottom
                    source: ResourceManager.icon(sortRow.sortImg, "png")
                    onClicked: sortRow.sort = !sortRow.sort
                }
            }
        }

        // ===== REFRESH =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            ImgButton {
                source: ResourceManager.icon("refresh", "png")
            }
        }

        // ===== SWITCH =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Icon {
                width: 26
                height: 26
                anchors.horizontalCenter: parent.horizontalCenter
                source: ResourceManager.icon("delete", "png")
                canChange: false
            }

            Switch {
                bottomPadding: 0

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                checked: false
            }
        }

        // ===== SETTINGS ICON =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            SettingsDialog {
                id: settingsDialog
            }


            ImgButton {
                imgScale: 1.2
                source: ResourceManager.icon("settings")
                onClicked: settingsDialog.open()
            }
        }

        // ===== ADD =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            ImgButton {
                imgScale: 0.8
                source: ResourceManager.icon("add", "png")
                onClicked: console.log("Add")
            }
        }
    }
}
