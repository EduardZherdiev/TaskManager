import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Button {
    id: control

    implicitHeight: 40
    implicitWidth: 140

    property int radius: Style.mediumRadius

    // ===== ENUM =====
    readonly property int defaultButton: 0
    readonly property int primaryButton: 1
    readonly property int errorButton: 2

    property int type: 0

    background: Rectangle {
        radius: control.radius
        color: control.enabled ? buttonColor() : Style.componentDisabled
        opacity: control.enabled ? Style.defaultOpacity : Style.disabledOpacity
    }

    contentItem: Text {
        text: control.text
        color: textColor()
        font.pixelSize: Style.smallFont
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    function buttonColor() {
        switch (control.type) {
            case control.primaryButton:
                return control.pressed
                    ? Style.primaryPressed
                    : control.hovered
                        ? Style.primaryHover
                        : Style.primaryColor
            case control.errorButton:
                return control.pressed
                    ? Style.errorPressed
                    : control.hovered
                        ? Style.errorHover
                        : Style.errorColor
            default:
                return control.pressed
                    ? Style.componentPressed
                    : control.hovered
                        ? Style.componentHover
                        : Style.componentBackground
        }
    }

    function textColor() {
        return control.enabled ? Style.componentText : Style.componentDisabledText
    }
}
