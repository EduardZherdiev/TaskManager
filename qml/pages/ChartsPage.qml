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
        spacing: Style.mediumSpacing

        Label {
            text: qsTr("Tasks Status Distribution")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Style.mediumSpacing

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: Style.smallSpacing
                Layout.fillHeight: false

                TabBar {
                    id: tabBar
                    Layout.preferredWidth: 150
                    background: Rectangle {
                        color: "transparent"
                    }
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
                    Layout.bottomMargin: Style.mediumSpacing

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

                Text {
                    text: tabBar.currentIndex
                          === 1 ? qsTr("Rotate: Arrow Keys") : ""
                    color: Style.textSecondaryColor
                    font.pixelSize: Style.smallFont
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }
                Text {
                    text: tabBar.currentIndex
                          === 1 ? qsTr("Zoom: +/-") : ""
                    color: Style.textSecondaryColor
                    font.pixelSize: Style.smallFont
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }
                Text {
                    text: tabBar.currentIndex
                          === 1 ? qsTr("Reset: Space") : ""
                    color: Style.textSecondaryColor
                    font.pixelSize: Style.smallFont
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                Text {
                    text: tabBar.currentIndex === 1 ? qsTr("Click on chart to restore focus") : ""
                    color: Style.textSecondaryColor
                    font.pixelSize: Style.smallFont
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }
            }


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

                        // Keyboard control: Arrow keys, +/-, Space
                        Keys.onPressed: event => {
                                            // ESC returns focus
                                            if (event.key === Qt.Key_Escape) {
                                                chart3dContainer.forceActiveFocus()
                                                event.accepted = true
                                                return
                                            }

                                            const rotationStep = 5.0
                                            const zoomStep = 0.1
                                            let handled = false

                                            // Arrow keys for rotation
                                            if (event.key === Qt.Key_Left) {
                                                barChart3d.yaw -= rotationStep
                                                handled = true
                                            } else if (event.key === Qt.Key_Right) {
                                                barChart3d.yaw += rotationStep
                                                handled = true
                                            } else if (event.key === Qt.Key_Up) {
                                                barChart3d.pitch -= rotationStep
                                                handled = true
                                            } else if (event.key === Qt.Key_Down) {
                                                barChart3d.pitch += rotationStep
                                                handled = true
                                            } // Plus/Equals for zoom in
                                            else if (event.key === Qt.Key_Plus
                                                     || event.key === Qt.Key_Equal) {
                                                barChart3d.scale += zoomStep
                                                handled = true
                                            } // Minus for zoom out
                                            else if (event.key === Qt.Key_Minus) {
                                                if (barChart3d.scale > zoomStep) {
                                                    barChart3d.scale -= zoomStep
                                                }
                                                handled = true
                                            } // Space to reset
                                            else if (event.key === Qt.Key_Space) {
                                                barChart3d.yaw = 30.0
                                                barChart3d.pitch = -20.0
                                                barChart3d.scale = 1.0
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
        }
    }
}
