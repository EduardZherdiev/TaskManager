import QtQuick 2.11
import QtQuick.Effects
import StyleModule 1.0

Item {
    id: root

    implicitWidth: 40
    implicitHeight: 40

    property url source
    property bool invertColors: Style.isDarkTheme
    property real brightnessValue: 1.0
    property real contrastValue: 1.0
    property real saturationValue: 1.0

    Image {
        id: icon
        anchors.fill: parent
        source: root.source
        smooth: true
    }

    MultiEffect {
        anchors.fill: parent
        source: icon
        visible: root.invertColors

        brightness: root.brightnessValue
        contrast: root.contrastValue
        saturation: root.saturationValue
    }
}
