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

    ToolTip.visible: hovered && tooltip !== ""
    ToolTip.text: tooltip
    ToolTip.delay: 500

}
