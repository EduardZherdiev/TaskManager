import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components

Dialog {
    id: dialog
    modal: true
    width: 600
    height: 500
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose
    padding: Style.mediumSpacing

    onOpened: {
        resolveAllButton.forceActiveFocus()
    }

    signal conflictResolved(int conflictId, bool useLocal)

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 1
        border.color: Style.componentOutline
    }

    Column {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        // ===== TITLE =====
        Label {
            width: parent.width
            text: qsTr("Sync Conflicts Detected")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            width: parent.width
            text: qsTr("Choose which version to keep for each conflict:")
            font.pixelSize: Style.smallFont
            color: Style.secondaryText
            wrapMode: Text.WordWrap
        }

        // ===== CONFLICTS LIST =====
        ScrollView {
            width: parent.width
            height: parent.height - 200
            clip: true

            Column {
                width: parent.width - 20
                spacing: Style.mediumSpacing

                Repeater {
                    id: conflictsRepeater
                    model: 0  // Будет установлено из C++

                    Rectangle {
                        width: parent.width
                        height: 150
                        color: Style.componentBackground
                        radius: Style.smallRadius
                        border.width: 1
                        border.color: Style.componentOutline

                        Column {
                            anchors.fill: parent
                            anchors.margins: Style.smallSpacing
                            spacing: Style.smallSpacing

                            // Заголовок конфликта
                            Label {
                                text: qsTr("Conflict in") + " " + model.itemType + " #" + model.id + " (" + model.field + ")"
                                font.bold: true
                                color: Style.textColor
                                font.pixelSize: Style.smallFont
                            }

                            Row {
                                width: parent.width
                                spacing: Style.mediumSpacing

                                // Локальное значение
                                Column {
                                    width: (parent.width - Style.mediumSpacing) / 2
                                    spacing: 5

                                    Label {
                                        text: qsTr("Local (Yours)")
                                        color: "#4CAF50"
                                        font.bold: true
                                        font.pixelSize: Style.smallFont
                                    }

                                    Rectangle {
                                        width: parent.width
                                        height: 40
                                        color: Style.surfaceColor
                                        radius: 4
                                        border.width: 1
                                        border.color: "#4CAF50"

                                        Text {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            text: String(model.localValue)
                                            color: Style.textColor
                                            wrapMode: Text.WordWrap
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                    }

                                    Button {
                                        width: parent.width
                                        text: qsTr("Keep Local")
                                        onClicked: {
                                            conflictResolved(model.index, true)
                                        }
                                    }
                                }

                                // Удалённое значение
                                Column {
                                    width: (parent.width - Style.mediumSpacing) / 2
                                    spacing: 5

                                    Label {
                                        text: qsTr("Remote (Server)")
                                        color: "#2196F3"
                                        font.bold: true
                                        font.pixelSize: Style.smallFont
                                    }

                                    Rectangle {
                                        width: parent.width
                                        height: 40
                                        color: Style.surfaceColor
                                        radius: 4
                                        border.width: 1
                                        border.color: "#2196F3"

                                        Text {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            text: String(model.remoteValue)
                                            color: Style.textColor
                                            wrapMode: Text.WordWrap
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                    }

                                    Button {
                                        width: parent.width
                                        text: qsTr("Keep Remote")
                                        onClicked: {
                                            conflictResolved(model.index, false)
                                        }
                                    }
                                }
                            }
                        }
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
                id: resolveAllButton
                text: qsTr("Resolve All")
                onClicked: dialog.accept()
            }

            Button {
                text: qsTr("Cancel")
                type: 1
                onClicked: dialog.reject()
            }
        }
    }

    function setConflicts(conflicts) {
        conflictsRepeater.model = conflicts
    }
}
