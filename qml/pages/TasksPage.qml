import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components
import core
import dialogs

Column {
    id: body
    spacing: 0

    property var columnWidths: [3, 2, 2, 2, 2]
        property real baseFixedButtonsWidth: 2*Style.smallSpacing + 72 + Style.mediumSpacing
        property real fixedButtonsWidth: TaskModel.showDeleted
            ? baseFixedButtonsWidth + Style.smallSpacing + 36
            : baseFixedButtonsWidth


        DeleteDialog {
            id: deleteDialog
            onConfirmed: function(taskId) {
                console.log("Confirmed deletion of task ID:", taskId)
                TaskModel.deleteTask(taskId)
            }
        }

        TaskDialog {
            id: taskDialog
            onSaveRequested: function(taskId, title, description, state) {
                console.log("Save requested", taskId, title, description, state)
                if (taskId === -1) {
                    // Create new task
                    TaskModel.createTask(title, description, state)
                } else {
                    // Update existing task
                    TaskModel.updateTask(taskId, title, description, state)
                }
            }
        }

        function columnWidth(index, totalWidth) {
            var activeWidths = []
            var activeIndices = [0, 1, 2, 4]  // Title, Created, Updated, State
            if (TaskModel.showDeleted) {
                activeIndices = [0, 1, 2, 3, 4]  // Include Deleted column
            }

            var sum = 0
            for (var i = 0; i < activeIndices.length; ++i) {
                sum += columnWidths[activeIndices[i]]
            }

            var availableWidth = totalWidth - fixedButtonsWidth
            return availableWidth * columnWidths[index] / sum
        }

        // ===== HEADER =====
        Row {
            width: parent.width
            height: 40
            spacing: 0

            Label {
                width: columnWidth(0, parent.width)
                height: parent.height
                text: qsTr("Title")
                color: Style.textColor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            Label {
                width: columnWidth(1, parent.width)
                height: parent.height
                text: qsTr("Created")
                color: Style.textColor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            Label {
                width: columnWidth(2, parent.width)
                height: parent.height
                text: qsTr("Updated")
                color: Style.textColor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            Label {
                width: columnWidth(3, parent.width)
                height: parent.height
                text: qsTr("Deleted")
                color: Style.textColor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                visible: TaskModel.showDeleted
            }
            Label {
                width: columnWidth(4, parent.width)
                height: parent.height
                text: qsTr("State")
                color: Style.textColor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            Label {
                width: columnWidth(4, parent.width)
                height: parent.height
                text: ""
                visible: false
            }
            Label {
                width: columnWidth(4, parent.width)
                height: parent.height
                text: ""
                visible: false
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: Style.componentOutline
        }

        // ===== BODY =====
        Item {
            width: parent.width
            height: parent.height - 41

            ListView {
                id: listView
                anchors.fill: parent
                topMargin: 10
                clip: true
                model: TaskModel
                spacing: Style.smallSpacing
                visible: listView.count > 0

            delegate: Row {
                width: listView.width
                height: 36
                spacing: 0

                Text {
                    width: columnWidth(0, listView.width)
                    height: 36
                    elide: Text.ElideRight
                    text: title || ""
                }
                Text {
                    width: columnWidth(1, listView.width)
                    height: 36
                    elide: Text.ElideRight
                    text: createdAt || ""
                }
                Text {
                    width: columnWidth(2, listView.width)
                    height: 36
                    elide: Text.ElideRight
                    text: updatedAt || ""
                }
                Text {
                    width: columnWidth(3, listView.width)
                    height: 36
                    elide: Text.ElideRight
                    text: deletedAt || ""
                    visible: TaskModel.showDeleted
                }

                ComboBox {
                    width: columnWidth(4, listView.width)
                    height: 36
                    currentIndex: {
                        var idx = taskState !== undefined ? Number(taskState) : 0
                        var maxCount = 3
                        if (model && model.length !== undefined)
                            maxCount = model.length
                        else if (model && model.count !== undefined)
                            maxCount = model.count
                        return Math.min(idx, maxCount - 1)
                    }
                    model: [qsTr("Active"), qsTr("Completed"), qsTr("Archived")]
                    onActivated: function(index) {
                        if (taskState !== index) {
                            TaskModel.updateTask(id, title, description, index)
                        }
                    }
                }

                // actions: two columns for buttons
                Item { width: Style.smallSpacing; height: 1 }
                ImgButton {
                    source: ResourceManager.icon("view","png")
                    height: parent.height
                    width: height
                    onClicked: {
                        taskDialog.openForTask(id, title, description, taskState, createdAt, updatedAt, deletedAt)
                    }
                }

                Item { width: Style.smallSpacing; height: 1 }

                ImgButton {
                    source: ResourceManager.icon("delete","png")
                    height: parent.height
                    width: height
                    type: 2
                    onClicked: {
                        if (TaskModel.showDeleted) {
                            deleteDialog.taskId = id
                            deleteDialog.taskTitle = title
                            deleteDialog.open()
                            return
                        }
                        TaskModel.softDeleteTask(id)
                    }
                }

                Item { width: Style.smallSpacing; height: 1 }

                ImgButton {
                    source: ResourceManager.icon("restore","png")
                    height: parent.height
                    width: height
                    visible: TaskModel.showDeleted
                    type: 1
                    onClicked: {
                        TaskModel.restoreTask(id)
                    }
                }

                Item { width: Style.mediumSpacing; height: 1 }
            }

            ScrollBar.vertical: ScrollBar {
                width:13
                policy: ScrollBar.AlwaysOn
            }
        }

        Label {
            anchors.centerIn: parent
            visible: listView.count === 0
            text: TaskModel.showDeleted
                ? qsTr("Here are no deleted records")
                : qsTr("Here are no active records")
            color: Style.textColor
            font.pixelSize: 16
            font.italic: true
        }
        }
    }
