import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

CheckBox {
    id: control

    property int fontSize: Style.smallFont
    property color textColor: Style.textColor

    contentItem: Text {
        text: control.text
        color: control.enabled ? control.textColor : Style.componentDisabledText
        font.pixelSize: control.fontSize
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        leftPadding: control.indicator.width + control.spacing
        elide: Text.ElideRight
    }

    indicator: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: Style.smallRadius
        border.color: control.enabled ? Style.controlBorder : Style.componentDisabled
        color: control.checked ? Style.primaryColor : Style.surfaceColor

        Rectangle {
            width: 12
            height: 12
            x: 4
            y: 4
            radius: Style.smallRadius
            color: Style.surfaceColor
            visible: control.checked
        }
    }
}
