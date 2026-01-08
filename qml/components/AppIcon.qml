import QtQuick 2.11
import QtQuick.Effects
import StyleModule 1.0

Item {
    id: root

    implicitWidth: 40
    implicitHeight: 40

    property url source: ""
    property bool canChange: true

    Image {
        id: icon
        anchors.fill: parent
        source: root.source
        visible: true
    }

    MultiEffect {
        anchors.fill: parent
        source: root.canChange ? icon : null

        colorization: 1.0
        colorizationColor: Style.textColor

        brightness: 0.0
        contrast: 1.0
        saturation: 0.0
    }
}
