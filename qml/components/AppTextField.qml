import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

TextField {
    id: control

    implicitHeight: 36
    implicitWidth: 200

    color: Style.textColor
    placeholderTextColor: Style.textSecondaryColor
    selectionColor: Style.primaryColor
    selectedTextColor: Style.textColor
    font.pixelSize: Style.mediumFont
    leftPadding: 10

    background: Rectangle {
        radius: Style.mediumRadius
        color: Style.controlBackground
        border.color: control.activeFocus ? Style.controlActiveBorder : Style.controlBorder
    }
}
