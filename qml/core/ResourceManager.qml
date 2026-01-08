pragma Singleton
import QtQuick 2.11

QtObject {
    property string localIconBase: "qrc:/assets/icons/"
    property string remoteIconBase: "https://cdn.example.com/icons/"

    property bool useRemote: false

    function icon(name,ext="svg") {
        var base = useRemote ? remoteIconBase : localIconBase
        return base + name + "."+ext
    }

    function image(name) {
        return "qrc:/assets/images/" + name + "."+ext
    }
}
