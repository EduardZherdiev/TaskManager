import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Label {
    id: control

    text: control.text
    font.pixelSize: Style.smallFont
    color: control.enabled ? Style.componentText : Style.componentDisabledText
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font.bold: true
}
