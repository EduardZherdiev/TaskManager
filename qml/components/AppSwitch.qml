import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0

Switch {
    id: control

    implicitWidth: 48
    implicitHeight: 24

    indicator: Rectangle {
        id: track
        width: 48
        height: 24
        radius: height / 2
        anchors.centerIn: parent

        color: control.checked
            ? Style.errorHover
            : Style.componentBackground

        // ===== CLICK ON TRACK =====
        MouseArea {
            anchors.fill: parent
            onClicked: control.checked = !control.checked
        }

        // ===== THUMB =====
        Rectangle {
            width: 20
            height: 20
            radius: width / 2
            y: 2
            x: control.checked
                ? parent.width - width - 2
                : 2

            color: Style.textColor

            Behavior on x {
                NumberAnimation {
                    duration: Style.fastAnimation
                }
            }
        }
    }
}
