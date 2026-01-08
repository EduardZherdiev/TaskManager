import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

RadioButton {
    id: radio

    property int fontSize: Style.smallFont
    property color textColor: Style.textColor

    contentItem: Text {
        text: radio.text
        color: radio.enabled ? radio.textColor : Style.componentDisabledText
        font.pixelSize: radio.fontSize
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        leftPadding: 20
        elide: Text.ElideRight
    }

}
