import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Text {
    id: control

    text: control.text
    color: control.enabled ? Style.componentText : Style.componentDisabledText
    font.pixelSize: Style.mediumFont
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
}
