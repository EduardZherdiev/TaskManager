import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components
import core

Column {
    id: body
    anchors.fill: parent
    spacing: 0

    property var columnWidths: [2, 1, 1, 1, 1, 1,1]

    function columnWidth(index, totalWidth) {
        var sum = 0
        for (var i = 0; i < columnWidths.length; ++i)
            sum += columnWidths[i]
        return totalWidth * columnWidths[index] / sum
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
                model: 5

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
                        case 4: {
                            switch (taskState) {
                            case 0: return "Active";
                            case 1: return "Completed";
                            case 2: return "Archived";
                            case 3: return "Deleted";
                            default: return taskState.toString();
                            }
                        }
                        default: return "";
                        }
                    }

                    text: value
                }
            }

            // actions: two columns for buttons
            ImgButton {
                source: ResourceManager.icon("view","png")
                height: parent.height
                width: height
                onClicked: console.log("View clicked for row", index)
            }

            ImgButton {
                source: ResourceManager.icon("delete","png")
                height: parent.height
                width: height
                type: 2
                onClicked: console.log("Delete clicked for row", index)
            }
        }

        ScrollBar.vertical: ScrollBar {
            width:13
            policy: ScrollBar.AlwaysOn
        }
    }
}
