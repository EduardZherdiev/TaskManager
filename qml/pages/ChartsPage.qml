import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleModule 1.0
import core

Rectangle {
    id: root
    color: Style.backgroundColor

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.bigSpacing
        spacing: Style.bigSpacing

        Label {
            text: qsTr("Tasks Status Distribution")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            Layout.alignment: Qt.AlignHCenter
        }

        // Chart area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 300
            color: "transparent"

            OpenGLBarChart {
                anchors.fill: parent
                completed: TaskModel.completedCount
                inProgress: TaskModel.inProgressCount
                archived: TaskModel.archivedCount
                completedColor: Style.statusCompleted
                inProgressColor: Style.statusInProgress
                archivedColor: Style.statusArchived
            }
        }

        Item {
            Layout.fillWidth: true
            height: 60

            Row {
                anchors.centerIn: parent
                spacing: 30

                Row {
                    spacing: 8
                    Rectangle {
                        width: 20
                        height: 20
                        color: Style.statusCompleted
                        radius: 2
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Completed: " + TaskModel.completedCount
                        font.pixelSize: 12
                        color: Style.textColor
                    }
                }

                Row {
                    spacing: 8
                    Rectangle {
                        width: 20
                        height: 20
                        color: Style.statusInProgress
                        radius: 2
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "In Progress: " + TaskModel.inProgressCount
                        font.pixelSize: 12
                        color: Style.textColor
                    }
                }

                Row {
                    spacing: 8
                    Rectangle {
                        width: 20
                        height: 20
                        color: Style.statusArchived
                        radius: 2
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Archived: " + TaskModel.archivedCount
                        font.pixelSize: 12
                        color: Style.textColor
                    }
                }
            }
        }
    }
}
