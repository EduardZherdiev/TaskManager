import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Window {
    objectName: "styleWindow"
    visible: true
    width: 560
    height: 760
    title: "Style preview"

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
                    text: "STYLE PREVIEW"
                    font.pixelSize: 24
                    color: Style.textColor
                }

                /* ===== COLORS ===== */
                SectionTitle { text: "COLORS" }

                ColorRow { name: "Primary";      color: Style.primaryColor }
                ColorRow { name: "Primary Var."; color: Style.primaryVariantColor }
                ColorRow { name: "Secondary";    color: Style.secondaryColor }
                ColorRow { name: "Error";        color: Style.errorColor }
                ColorRow { name: "Background";  color: Style.backgroundColor }
                ColorRow { name: "Surface";      color: Style.surfaceColor }
                ColorRow { name: "Text";         color: Style.textColor }
                ColorRow { name: "Text Sec.";    color: Style.textSecondaryColor }

                /* ===== OPACITY ===== */
                SectionTitle { text: "OPACITY" }

                OpacityRow { label: "Default";   value: Style.defaultOpacity }
                OpacityRow { label: "Emphasis";  value: Style.emphasisOpacity }
                OpacityRow { label: "Secondary"; value: Style.secondaryOpacity }
                OpacityRow { label: "Disabled";  value: Style.disabledOpacity }

                /* ===== COMPONENT STATES ===== */
                SectionTitle { text: "COMPONENT STATES" }

                Row {
                    spacing: Style.mediumSpacing

                    PreviewButton {
                        text: "Normal"
                        bgColor: Style.componentBackground
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: "Hover"
                        bgColor: Style.componentHover
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: "Pressed"
                        bgColor: Style.componentPressed
                        textColor: Style.componentText
                    }

                    PreviewButton {
                        text: "Disabled"
                        bgColor: Style.componentDisabled
                        textColor: Style.componentDisabledText
                    }
                }

                /* ===== SPACING ===== */
                SectionTitle { text: "SPACING" }

                SpacingRow { label: "Small";  value: Style.smallSpacing }
                SpacingRow { label: "Medium"; value: Style.mediumSpacing }
                SpacingRow { label: "Big";    value: Style.bigSpacing }
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
