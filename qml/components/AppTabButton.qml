import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

TabButton {
    id: control

    implicitHeight: 32
    implicitWidth: 100

    background: Rectangle {
        radius: Style.mediumRadius
        color: control.checked
               ? (control.pressed ? Style.primaryPressed : Style.primaryColor)
               : (control.hovered ? Style.componentHover : Style.componentBackground)
        border.color: control.activeFocus ? Style.componentFocusRing : Style.componentOutline
        opacity: control.enabled ? 1.0 : Style.disabledOpacity
    }

    contentItem: Text {
        text: control.text
        font.pixelSize: Style.smallFont
        color: control.enabled
               ? (control.checked ? Style.textColor : Style.componentText)
               : Style.componentDisabledText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
