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
        border.color: Style.componentOutline
        border.width: 1
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.smallSpacing
        spacing: Style.mediumSpacing

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
            TabButton { text: qsTr("View")  }
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
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

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
                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 160
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    
                    TextArea {
                        id: descriptionField
                        Layout.fillWidth: true
                        width: parent.width - 8
                        placeholderText: qsTr("Description")
                        text: taskDialog.taskDescription
                        wrapMode: TextEdit.Wrap
                    }
                }
                ComboBox {
                    id: stateCombo
                    Layout.fillWidth: true
                    model: [qsTr("Active"), qsTr("Completed"), qsTr("Archived"), qsTr("Deleted")]
                    currentIndex: taskDialog.taskState
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
        case 3: return qsTr("Deleted");
        default: return qsTr("Unknown");
        }
    }
}
