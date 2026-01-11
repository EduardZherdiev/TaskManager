import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components

Dialog {
    id: taskDialog
    modal: true
    width: 520
    height: 420
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose

    property int taskId: -1
    property string taskTitle: ""
    property string taskDescription: ""
    property int taskState: 0
    property string createdAt: ""
    property string updatedAt: ""
    property string deletedAt: ""

    signal saveRequested(int taskId, string title, string description, int taskState)

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 0 // remove outline line under buttons
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.smallSpacing
        spacing: Style.smallSpacing

        // Title
        Label {
            text: qsTr("Task details")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            Layout.alignment: Qt.AlignHCenter
        }

        TabBar {
            id: tabs
            Layout.fillWidth: true
            currentIndex: 0
            spacing: Style.smallSpacing
            background: Item {}

            TabButton {
                text: qsTr("View")
                enabled: taskDialog.taskId !== -1
            }
            TabButton { text: qsTr("Edit") }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabs.currentIndex


            // View tab
            ColumnLayout {
                Layout.fillWidth: true
                spacing: Style.smallSpacing
                Label { 
                    Layout.fillWidth: true
                    text: qsTr("Title: ") + taskDialog.taskTitle
                    color: Style.textColor
                    horizontalAlignment: Text.AlignLeft
                }
                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                    ScrollBar.vertical.active: true
                    ScrollBar.vertical.width: 13
                    ScrollBar.vertical.contentItem: Rectangle {
                        implicitWidth: 6
                        radius: 3
                        color: Style.componentBackground
                    }
                    ScrollBar.vertical.background: Rectangle {
                        color: "transparent"
                    }

                    Label {
                        width: parent.width - 8
                        text: qsTr("Description: ") + taskDialog.taskDescription
                        color: Style.textColor
                        wrapMode: Text.Wrap
                        horizontalAlignment: Text.AlignLeft
                    }
                }
                Label { 
                    Layout.fillWidth: true
                    text: qsTr("State: ") + stateToText(taskDialog.taskState)
                    color: Style.textColor
                    horizontalAlignment: Text.AlignLeft
                }
                Label { 
                    Layout.fillWidth: true
                    text: qsTr("Created: ") + taskDialog.createdAt
                    color: Style.textColor
                    horizontalAlignment: Text.AlignLeft
                }
                Label { 
                    Layout.fillWidth: true
                    text: qsTr("Updated: ") + taskDialog.updatedAt
                    color: Style.textColor
                    horizontalAlignment: Text.AlignLeft
                }
                Label { 
                    Layout.fillWidth: true
                    text: qsTr("Deleted: ") + taskDialog.deletedAt
                    color: Style.textColor
                    horizontalAlignment: Text.AlignLeft
                }
            }

            // Edit tab
            ColumnLayout {
                spacing: Style.smallSpacing
                Layout.fillWidth: true
                TextField {
                    id: titleField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Title")
                    text: taskDialog.taskTitle
                }
                Flickable {
                    id: flickable
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    clip: true
                    contentWidth: descriptionField.width
                    contentHeight: descriptionField.contentHeight + 20
                    
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                        width: 13
                        contentItem: Rectangle {
                            implicitWidth: 6
                            radius: 3
                            color: Style.componentBackground
                        }
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                    
                    TextArea {
                        id: descriptionField
                        width: flickable.width
                        height: Math.max(flickable.height, contentHeight + 20)
                        text: taskDialog.taskDescription
                        placeholderText: qsTr("Description")
                        wrapMode: TextEdit.Wrap
                        background: Rectangle {
                            color: Style.controlBackground
                            border.color: descriptionField.activeFocus ? Style.controlActiveBorder : Style.controlBorder
                            radius: Style.mediumRadius
                        }
                    }
                }
                ComboBox {
                    id: stateCombo
                    Layout.fillWidth: true
                    model: [qsTr("Active"), qsTr("Completed"), qsTr("Archived")]
                    currentIndex: Math.min(taskDialog.taskState, model.count - 1)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.mediumSpacing
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("Close")
                onClicked: taskDialog.close()
            }
            Button {
                text: qsTr("Save")
                type: 2
                onClicked: {
                    taskDialog.saveRequested(taskDialog.taskId, titleField.text, descriptionField.text, stateCombo.currentIndex)
                    taskDialog.close()
                }
            }
        }
    }

    function openForTask(id, title, description, state, created, updated, deleted) {
        taskId = id
        taskTitle = title || ""
        taskDescription = description || ""
        taskState = state || 0
        createdAt = created || ""
        updatedAt = updated || ""
        deletedAt = deleted || ""
        titleField.text = taskTitle
        descriptionField.text = taskDescription
        stateCombo.currentIndex = taskState
        // Open in Edit tab for new tasks, View tab for existing
        tabs.currentIndex = (taskId === -1) ? 1 : 0
        open()
    }

    function stateToText(s) {
        switch (s) {
        case 0: return qsTr("Active");
        case 1: return qsTr("Completed");
        case 2: return qsTr("Archived");
        default: return qsTr("Unknown");
        }
    }
}
