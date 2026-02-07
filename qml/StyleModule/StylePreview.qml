import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Window {
    objectName: "styleWindow"
    visible: true
    width: 560
    height: 760
    title: qsTr("Style preview")

    Rectangle {
        anchors.fill: parent
        color: Style.backgroundColor

        Flickable {
            anchors.fill: parent
            contentHeight: column.implicitHeight + 20

            Column {
                id: column
                width: parent.width
                spacing: Style.bigSpacing
                padding: Style.defaultOffset

                /* ===== HEADER ===== */
                Text {
                    text: qsTr("STYLE PREVIEW")
                    font.pixelSize: 24
                    color: Style.textColor
                }

                /* ===== COLORS ===== */
                SectionTitle { text: qsTr("COLORS") }

                ColorRow { name: qsTr("Primary");      color: Style.primaryColor }
                ColorRow { name: qsTr("Primary Var."); color: Style.primaryVariantColor }
                ColorRow { name: qsTr("Secondary");    color: Style.secondaryColor }
                ColorRow { name: qsTr("Error");        color: Style.errorColor }
                ColorRow { name: qsTr("Background");  color: Style.backgroundColor }
                ColorRow { name: qsTr("Surface");      color: Style.surfaceColor }
                ColorRow { name: qsTr("Text");         color: Style.textColor }
                ColorRow { name: qsTr("Text Sec.");    color: Style.textSecondaryColor }

                /* ===== OPACITY ===== */
                SectionTitle { text: qsTr("OPACITY") }

                OpacityRow { label: qsTr("Default");   value: Style.defaultOpacity }
                OpacityRow { label: qsTr("Emphasis");  value: Style.emphasisOpacity }
                OpacityRow { label: qsTr("Secondary"); value: Style.secondaryOpacity }
                OpacityRow { label: qsTr("Disabled");  value: Style.disabledOpacity }

                /* ===== COMPONENT STATES ===== */
                SectionTitle { text: qsTr("COMPONENT STATES") }

                Row {
                    spacing: Style.mediumSpacing

                    PreviewButton {
                        text: qsTr("Normal")
                        bgColor: Style.componentBackground
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: qsTr("Hover")
                        bgColor: Style.componentHover
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: qsTr("Pressed")
                        bgColor: Style.componentPressed
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: qsTr("Disabled")
                        bgColor: Style.componentDisabled
                        textColor: Style.componentDisabledText
                    }
                }

                /* ===== SPACING ===== */
                SectionTitle { text: qsTr("SPACING") }

                SpacingRow { label: qsTr("Small");  value: Style.smallSpacing }
                SpacingRow { label: qsTr("Medium"); value: Style.mediumSpacing }
                SpacingRow { label: qsTr("Big");    value: Style.bigSpacing }
            }
        }
    }

    /* ====== COMPONENTS ====== */

    component SectionTitle: Text {
        font.pixelSize: 20
        color: Style.textColor
    }

    component ColorRow: Row {
        spacing: Style.mediumSpacing

        property string name
        property color color

        Rectangle {
            width: 60
            height: 40
            radius: Style.smallRadius
            color: parent.color
            border.color: Style.componentOutline
        }

        Text {
            text: name + "  " + parent.color
            color: Style.textColor
            verticalAlignment: Text.AlignVCenter
        }
    }

    component OpacityRow: Row {
        spacing: Style.mediumSpacing
        property string label
        property real value

        Rectangle {
            width: 60
            height: 40
            radius: Style.smallRadius
            color: Style.primaryColor
            opacity: value
        }

        Text {
            text: label + ": " + value
            color: Style.textColor
            verticalAlignment: Text.AlignVCenter
        }
    }

    component PreviewButton: Rectangle {
        width: 120
        height: 44
        radius: Style.mediumRadius

        property color bgColor
        property color textColor
        property string text

        color: bgColor

        Text {
            anchors.centerIn: parent
            text: parent.text
            color: parent.textColor
        }
    }

    component SpacingRow: Row {
        spacing: Style.mediumSpacing
        property string label
        property int value

        Rectangle {
            width: value * 3
            height: 10
            color: Style.secondaryColor
            radius: 3
        }

        Text {
            text: label + ": " + value
            color: Style.textColor
        }
    }
}
