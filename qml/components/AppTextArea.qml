import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

TextArea {
    id: control

    implicitHeight: 160
    implicitWidth: 200

    color: Style.textColor
    placeholderTextColor: Style.textSecondaryColor
    selectionColor: Style.primaryColor
    selectedTextColor: Style.textColor
    wrapMode: TextEdit.Wrap
    leftPadding: 10
    rightPadding: 10
    topPadding: 8
    bottomPadding: 8

    background: Rectangle {
        radius: Style.mediumRadius
        color: Style.controlBackground
        border.color: control.activeFocus ? Style.controlActiveBorder : Style.controlBorder
    }

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }
}
