import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

ScrollView {
    id: scrollView
    
    implicitHeight: 160
    implicitWidth: 200
    
    // Expose TextArea properties
    property alias text: control.text
    property alias placeholderText: control.placeholderText
    property alias wrapMode: control.wrapMode
    property alias color: control.color
    property alias readOnly: control.readOnly
    property alias selectByMouse: control.selectByMouse
    
    ScrollBar.vertical.policy: ScrollBar.AsNeeded
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    
    TextArea {
        id: control

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
    }
}
