import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleModule 1.0
import components
import core

Rectangle {
    id: root
    color: Style.backgroundColor
    
    // Trigger animation when this page becomes current in SwipeView
    SwipeView.onIsCurrentItemChanged: {
        if (SwipeView.isCurrentItem) {
            barChart2d.restartAnimation()
        }
    }

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

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.mediumSpacing

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: Style.smallSpacing
                
                TabBar {
                    id: tabBar
                    Layout.preferredWidth: 150
                    background: Rectangle { color: "transparent" }
                    spacing: Style.smallSpacing
                    
                    onCurrentIndexChanged: {
                        // Trigger 2D animation when switching to 2D tab
                        if (currentIndex === 0) {
                            barChart2d.restartAnimation()
                        }
                    }
                    
                    TabButton {
                        text: qsTr("2D")
                        width: 60
                    }
                    TabButton {
                        text: qsTr("3D")
                        width: 60
                    }
                }
                
                // Legend below TabBar
                Column {
                    spacing: Style.smallSpacing
                    Layout.topMargin: Style.mediumSpacing
                    
                    Row {
                        spacing: 8
                        Rectangle {
                            width: 16
                            height: 16
                            color: Style.statusCompleted
                            radius: 2
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Completed: " + TaskModel.completedCount
                            font.pixelSize: 11
                            color: Style.textColor
                        }
                    }

                    Row {
                        spacing: 8
                        Rectangle {
                            width: 16
                            height: 16
                            color: Style.statusInProgress
                            radius: 2
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "In Progress: " + TaskModel.inProgressCount
                            font.pixelSize: 11
                            color: Style.textColor
                        }
                    }

                    Row {
                        spacing: 8
                        Rectangle {
                            width: 16
                            height: 16
                            color: Style.statusArchived
                            radius: 2
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Archived: " + TaskModel.archivedCount
                            font.pixelSize: 11
                            color: Style.textColor
                        }
                    }
                }
            }

            // Item { Layout.fillWidth: true }

            // Chart area
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 320
                color: "transparent"

                StackLayout {
                    anchors.fill: parent
                    currentIndex: tabBar.currentIndex

                    // 2D chart
                    OpenGLBarChart {
                        id: barChart2d
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        completed: TaskModel.completedCount
                        inProgress: TaskModel.inProgressCount
                        archived: TaskModel.archivedCount
                        completedColor: Style.statusCompleted
                        inProgressColor: Style.statusInProgress
                        archivedColor: Style.statusArchived
                    }

                    // 3D chart
                    Rectangle {
                        id: chart3dContainer
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "transparent"
                        property real startYaw: 0
                        property real startPitch: 0
                        focus: tabBar.currentIndex === 1

                        // Keyboard control: A/D/W/S keys (layout-independent)
                        Keys.onPressed: (event) => {
                            // ESC or click on graph returns focus
                            if (event.key === Qt.Key_Escape) {
                                chart3dContainer.forceActiveFocus()
                                event.accepted = true
                                return
                            }

                            const rotationStep = 5.0
                            let handled = false

                            // Check by both key code and text to support any keyboard layout
                            const lowerText = event.text.toLowerCase()

                            if (lowerText === "a" || lowerText === "ф" || event.key === Qt.Key_A) {
                                barChart3d.yaw -= rotationStep
                                handled = true
                            } else if (lowerText === "d" || lowerText === "в" || event.key === Qt.Key_D) {
                                barChart3d.yaw += rotationStep
                                handled = true
                            } else if (lowerText === "w" || lowerText === "ц" || event.key === Qt.Key_W) {
                                barChart3d.pitch -= rotationStep
                                handled = true
                            } else if (lowerText === "s" || lowerText === "ы" || event.key === Qt.Key_S) {
                                barChart3d.pitch += rotationStep
                                handled = true
                            }

                            if (handled) {
                                event.accepted = true
                            }
                        }

                        OpenGLBarChart3D {
                            id: barChart3d
                            anchors.fill: parent
                            completed: TaskModel.completedCount
                            inProgress: TaskModel.inProgressCount
                            archived: TaskModel.archivedCount
                            completedColor: Style.statusCompleted
                            inProgressColor: Style.statusInProgress
                            archivedColor: Style.statusArchived

                            // Click to restore focus
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    chart3dContainer.forceActiveFocus()
                                }
                            }
                        }
                    }
                }
            }


            Text {
                text: tabBar.currentIndex === 1 ? qsTr("Rotate: A/D/W/S • Click on chart to restore focus") : ""
                color: Style.textSecondaryColor
                font.pixelSize: Style.smallFont
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }

     }
}
