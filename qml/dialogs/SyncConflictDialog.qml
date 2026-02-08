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
        applyRemoteButton.forceActiveFocus()
    }

    signal resolveRequested(var ids, bool useRemote)

    property bool updatingSelectAll: false
    property int lastFocusIndex: -1

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

        Item {
            width: parent.width
            height: 24

            Label {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Choose which version to keep for each conflict:")
                font.pixelSize: Style.smallFont
                wrapMode: Text.WordWrap
            }

            CheckBox {
                id: selectAllCheck
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Select all")
                onToggled: {
                    if (updatingSelectAll) {
                        return
                    }
                    for (var i = 0; i < conflictsModel.count; i++) {
                        conflictsModel.setProperty(i, "selected", checked)
                    }
                }
            }
        }

        // ===== CONFLICTS LIST =====
        ScrollView {
            id: conflictsScroll
            width: dialog.width - dialog.padding * 2
            height: parent.height - 200
            clip: true

            Column {
                id: conflictsColumn
                width: conflictsScroll.width
                spacing: Style.smallSpacing
                property int checkboxWidth: 40
                property real columnWidth: Math.max(140, (width - checkboxWidth - Style.smallSpacing * 2) / 2)

                Row {
                    width: conflictsColumn.width
                    spacing: Style.smallSpacing

                    Label {
                        width: conflictsColumn.checkboxWidth
                        text: qsTr("Pick")
                        font.pixelSize: Style.smallFont
                    }

                    Label {
                        width: conflictsColumn.columnWidth
                        text: qsTr("Local tasks")
                        font.pixelSize: Style.smallFont
                    }

                    Label {
                        width: conflictsColumn.columnWidth
                        text: qsTr("Server tasks")
                        font.pixelSize: Style.smallFont
                    }
                }

                Repeater {
                    id: conflictsRepeater
                    model: conflictsModel

                    Row {
                        id: conflictRow
                        width: conflictsColumn.width
                        height: 35
                        spacing: Style.smallSpacing

                        function focusCheckbox() {
                            rowCheckBox.forceActiveFocus()
                        }

                        Item {
                            width: conflictsColumn.checkboxWidth
                            height: parent.height

                            CheckBox {
                                id: rowCheckBox
                                anchors.centerIn: parent
                                checked: selected
                                onToggled: {
                                    conflictsModel.setProperty(index, "selected", checked)
                                    updateSelectAll()
                                }
                            }
                        }

                        Rectangle {
                            width: conflictsColumn.columnWidth
                            height: 35
                            color: (localTitleMouse.containsMouse && !localTitleMouse.ignoreHover)
                                ? Style.componentHover
                                : Style.surfaceColor
                            radius: 4
                            border.width: 0

                            Text {
                                anchors.fill: parent
                                anchors.margins: 8
                                text: localTitle
                                color: Style.textColor
                                font.bold: true
                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignVCenter
                            }

                            MouseArea {
                                id: localTitleMouse
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                property bool ignoreHover: false
                                onClicked: {
                                    ignoreHover = true
                                    dialog.lastFocusIndex = index
                                    viewTaskDialog.readOnlyMode = true
                                    viewTaskDialog.openForTask(localId, localTitle, localDescription, localState, localCreatedAt, localUpdatedAt, localDeletedAt)
                                }
                                onExited: {
                                    ignoreHover = false
                                }
                            }
                        }

                        Rectangle {
                            width: conflictsColumn.columnWidth
                            height: 35
                            color: (remoteTitleMouse.containsMouse && !remoteTitleMouse.ignoreHover)
                                ? Style.componentHover
                                : Style.surfaceColor
                            radius: 4
                            border.width: 0

                            Text {
                                anchors.fill: parent
                                anchors.margins: 8
                                text: remoteTitle
                                color: Style.textColor
                                font.bold: true
                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignVCenter
                            }

                            MouseArea {
                                id: remoteTitleMouse
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                property bool ignoreHover: false
                                onClicked: {
                                    ignoreHover = true
                                    dialog.lastFocusIndex = index
                                    viewTaskDialog.readOnlyMode = true
                                    viewTaskDialog.openForTask(remoteId, remoteTitle, remoteDescription, remoteState, remoteCreatedAt, remoteUpdatedAt, remoteDeletedAt)
                                }
                                onExited: {
                                    ignoreHover = false
                                }
                            }
                        }
                    }
                }
            }
        }

        // ===== BUTTONS =====
        Row {
            width: implicitWidth
            spacing: Style.mediumSpacing
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("Close")
                type: 1
                onClicked: {
                    dialog.reject()
                }
            }

            Button {
                text: qsTr("Keep Local")
                onClicked: {
                    var ids = selectedIds()
                    if (ids.length === 0) {
                        return
                    }
                    resolveRequested(ids, false)
                }
            }

            Button {
                id: applyRemoteButton
                text: qsTr("Apply Server")
                onClicked: {
                    var ids = selectedIds()
                    if (ids.length === 0) {
                        return
                    }
                    resolveRequested(ids, true)
                }
            }
        }
    }

    function setConflicts(conflicts) {
        conflictsModel.clear()
        for (var i = 0; i < conflicts.length; i++) {
            var item = conflicts[i]
            item.selected = false
            conflictsModel.append(item)
        }
        updatingSelectAll = true
        selectAllCheck.checked = false
        updatingSelectAll = false
    }

    function selectedIds() {
        var ids = []
        for (var i = 0; i < conflictsModel.count; i++) {
            var item = conflictsModel.get(i)
            if (item.selected) {
                ids.push(item.id)
            }
        }
        return ids
    }

    function updateSelectAll() {
        if (conflictsModel.count === 0) {
            updatingSelectAll = true
            selectAllCheck.checked = false
            updatingSelectAll = false
            return
        }

        var allSelected = true
        for (var i = 0; i < conflictsModel.count; i++) {
            if (!conflictsModel.get(i).selected) {
                allSelected = false
                break
            }
        }

        updatingSelectAll = true
        selectAllCheck.checked = allSelected
        updatingSelectAll = false
    }

    ListModel {
        id: conflictsModel
    }

    TaskDialog {
        id: viewTaskDialog
        readOnlyMode: true
        onSaveRequested: {}
        onClosed: {
            if (dialog.lastFocusIndex >= 0) {
                var rowItem = conflictsRepeater.itemAt(dialog.lastFocusIndex)
                if (rowItem && rowItem.focusCheckbox) {
                    rowItem.focusCheckbox()
                }
            }
        }
    }
}
