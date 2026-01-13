import QtQuick.Controls
import QtQuick 2.11
import StyleModule 1.0
import QtQuick.Layouts

TabBar {
    id: tabBar
    height: 40

    background: Rectangle {
        color: Style.surfaceColor
    }

    padding: 0
    spacing: 0

    TabButton {
        text: qsTr("Tasks Table")
        Layout.fillWidth: true
        height: tabBar.height
        
        background: Rectangle {
            color: parent.hovered ? Style.componentHover : Style.surfaceColor
        }
        
        contentItem: Text {
            text: parent.text
            font.pixelSize: Style.mediumFont
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.centerIn: parent
        }
    }

    TabButton {
        text: qsTr("Progress Charts")
        Layout.fillWidth: true
        height: tabBar.height
        
        background: Rectangle {
            color:parent.hovered ? Style.componentHover : Style.surfaceColor
        }
        
        contentItem: Text {
            text: parent.text
            font.pixelSize: Style.mediumFont
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.centerIn: parent
        }
    }
}

