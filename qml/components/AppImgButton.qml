import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

AppButton {
    id: btn
    width: 40
    height: 40

    property double imgScale : 0.9
    property string tooltip: ""

    property url source: ""

    contentItem: AppIcon{

        scale: btn.imgScale
        source: btn.source
        invertColors: false
    }

    ToolTip {
        id: imageToolTip
        parent: btn
        visible: btn.hovered && tooltip !== ""
        text: tooltip
        delay: 500
        timeout: 5000
        padding: Style.tinyOffset
        x: (btn.width - implicitWidth) / 2
        y: -implicitHeight - Style.tinyOffset

        background: Rectangle {
            color: Style.tooltipBackground
            radius: Style.smallRadius
            border.color: Style.componentOutline
            border.width: 1
        }

        contentItem: Text {
            text: imageToolTip.text
            color: Style.tooltipTextColor
            font.pixelSize: Style.smallFont
        }
    }
}
