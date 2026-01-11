pragma Singleton

import QtQuick 2.11
import QtCore

QtObject {
    id: root

    property Settings settings: Settings {
        category: "Application"
        property alias isDarkTheme: root.isDarkTheme
        property alias language: root.language
        property alias storageMode: root.storageMode
        property alias rememberLogin: root.rememberLogin
        property alias savedLogin: root.savedLogin
        property alias savedPassword: root.savedPassword
    }

    // Theme: true = dark, false = light
    property bool isDarkTheme: true

    // Language: 0 = English, 1 = Українська, 2 = Deutsch
    property int language: 0

    // Storage: 0 = local, 1 = global
    property int storageMode: 0

    // Remember login
    property bool rememberLogin: false
    property string savedLogin: ""
    property string savedPassword: ""

    // Apply theme on startup
    Component.onCompleted: {
        console.log("AppSettings loaded: theme=" + isDarkTheme + ", lang=" + language + ", storage=" + storageMode)
    }
}
