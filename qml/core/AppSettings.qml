pragma Singleton

import QtQuick 2.11
import QtCore

QtObject {
    id: root

    property Settings settings: Settings {
        category: "Application"
        property alias isDarkTheme: root.isDarkTheme
        property alias language: root.language
        property alias rememberLogin: root.rememberLogin
        property alias savedLogin: root.savedLogin
        property alias savedPasswordHash: root.savedPasswordHash
        property alias savedAccessToken: root.savedAccessToken
        property alias savedRefreshToken: root.savedRefreshToken
    }

    // Theme: true = dark, false = light
    property bool isDarkTheme: true

    // Language: 0 = English, 1 = Українська, 2 = Deutsch
    property int language: 0

    // Remember login
    property bool rememberLogin: false
    property string savedLogin: ""
    property string savedPasswordHash: ""
    property string savedAccessToken: ""
    property string savedRefreshToken: ""

    // Apply theme on startup
    Component.onCompleted: {
        console.log("AppSettings loaded: theme=" + isDarkTheme + ", lang=" + language + ", rememberLogin=" + rememberLogin)
    }
}
