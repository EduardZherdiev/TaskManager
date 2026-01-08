import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts
import StyleModule 1.0
import include
import components

ApplicationWindow {

    id: root
    visible: true
    width: 1000
    height: 700
    title: qsTr("Task manager")
    property var styleWindow: null

    Rectangle {
        id: _background
        z: -100
        anchors.fill: parent
        color: Style.backgroundColor
    }

    header: Header{}

    Body{}

    footer: Footer{}

    // Item {
    //     anchors.fill: parent

    //     Button {
    //         text: "Show stylesheet"
    //         anchors.centerIn: parent

    //         onClicked: {
    //             if (!styleWindow) {
    //                 var component = Qt.createComponent(
    //                             "qml/StyleModule/StylePreview.qml")
    //                 if (component.status === Component.Ready) {
    //                     styleWindow = component.createObject(null)
    //                     styleWindow.visible = true
    //                 } else {
    //                     console.log("Error loading StylePreview:",
    //                                 component.errorString())
    //                 }
    //             } else {
    //                 styleWindow.visible = true
    //                 styleWindow.raise()
    //             }
    //         }
    //     }
    // }
}
