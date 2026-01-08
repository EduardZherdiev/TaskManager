import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import core

ComboBox {
    id: control

    implicitHeight: 25
    implicitWidth: 100

    indicator:
            Icon {
                width: 19
                height: 18
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 4
                source: ResourceManager.icon("arrow-down", "png")
            }


    background: Rectangle {
        radius: Style.mediumRadius
        color: control.enabled
            ? (control.pressed
                ? Style.componentPressed
                : control.hovered
                    ? Style.componentHover
                    : Style.componentBackground)
            : Style.componentDisabled

        border.color: control.activeFocus
            ? Style.componentFocusRing
            : Style.componentOutline
    }

    contentItem: Text {
        text: control.displayText
        color: Style.textColor
        verticalAlignment: Text.AlignVCenter
        leftPadding: 10
    }

    delegate: ItemDelegate {
        width: control.width
        height: 25
        text: modelData
        hoverEnabled: true

        background: Rectangle {
            color: highlighted || hovered
                ? Style.componentHover
                : Style.componentBackground

            Icon {
                width: 19
                height: 18
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 4

                source: ResourceManager.icon("arrow-up", "png")
                visible: index === 0
            }
        }


        contentItem: Text {
            text: modelData
            color: Style.textColor
            verticalAlignment: Text.AlignVCenter
            leftPadding: 10
        }
    }


    popup.background: Rectangle {
        radius: Style.smallRadius
        color: Style.surfaceColor
        border.color: Style.componentOutline
    }
}

