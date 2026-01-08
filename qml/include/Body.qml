import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components
import core
import dialogs

Column {
    id: body
    anchors.fill: parent
    spacing: 0

    property var columnWidths: [3, 2, 2, 2, 2]
    property real fixedButtonsWidth: 2*Style.smallSpacing + 72 + Style.mediumSpacing

    DeleteDialog {
        id: deleteDialog
        onConfirmed: function(taskId) {
            console.log("Confirmed deletion of task ID:", taskId)
            // TODO: Call TaskModel method to delete task
        }
    }

    TaskDialog {
        id: taskDialog
        onSaveRequested: function(taskId, title, description, state) {
            console.log("Save requested", taskId, title, description, state)
            // TODO: propagate to model/repository
        }
    }

    function columnWidth(index, totalWidth) {
        var sum = 0
        for (var i = 0; i < columnWidths.length; ++i)
            sum += columnWidths[i]
        var availableWidth = totalWidth - fixedButtonsWidth
        return availableWidth * columnWidths[index] / sum
    }

    // ===== HEADER =====
    Row {
        width: parent.width
        height: 40
        spacing: 0

        Repeater {
            model: ["Title", "Created", "Updated", "Deleted", "State", "",""]

            Label {
                width: columnWidth(index, parent.width)
                height: parent.height

                text: modelData
                color: Style.textColor
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
        }
    }

    Rectangle {
        width: parent.width
        height: 1
        color: Style.componentOutline
    }

    // ===== BODY =====
    ListView {
        id: listView
        width: parent.width
        height: parent.height - 41
        topMargin: 10
        clip: true
        model: TaskModel{}
        spacing: Style.smallSpacing

        delegate: Row {
            width: listView.width
            height: 36
            spacing: 0

            Repeater {
                model: 4

                Text {
                    width: columnWidth(index, listView.width)
                    height: 36
                    elide: Text.ElideRight

                    property string value: {
                        switch (index) {
                        case 0: return title || "";
                        case 1: return createdAt || "";
                        case 2: return updatedAt || "";
                        case 3: return deletedAt || "";
                        default: return "";
                        }
                    }

                    text: value
                }
            }

            ComboBox {
                width: columnWidth(4, listView.width)
                height: 36
                currentIndex: taskState !== undefined ? taskState : 0
                model: ["Active", "Completed", "Archived", "Deleted"]
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
                    deleteDialog.taskId = id
                    deleteDialog.taskTitle = title
                    deleteDialog.open()
                }
            }

            Item { width: Style.mediumSpacing; height: 1 }
        }

        ScrollBar.vertical: ScrollBar {
            width:13
            policy: ScrollBar.AlwaysOn
        }
    }
}
