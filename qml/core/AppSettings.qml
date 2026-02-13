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

    // Task filters and sorting
    property int filterMonth: -1
    property int sortField: 0
    property bool sortAscending: true

    // Remember login
    property bool rememberLogin: false
    property string savedLogin: ""
    property string savedPasswordHash: ""
    property string savedAccessToken: ""
    property string savedRefreshToken: ""

    onFilterMonthChanged: {
        settings.setValue("filterMonth", filterMonth)
    }
    
    onSortFieldChanged: {
        settings.setValue("sortField", sortField)
    }
    
    onSortAscendingChanged: {
        settings.setValue("sortAscending", sortAscending)
    }

    // Apply theme on startup
    Component.onCompleted: {
        if (language > 1) {
            language = 0
        }
        
        // Load filter and sort settings from QSettings
        var savedFilterMonth = settings.value("filterMonth", -1)
        if (savedFilterMonth !== undefined && savedFilterMonth !== null && savedFilterMonth !== "") {
            filterMonth = parseInt(savedFilterMonth)
        }
        
        var savedSortField = settings.value("sortField", 0)
        if (savedSortField !== undefined && savedSortField !== null && savedSortField !== "") {
            sortField = parseInt(savedSortField)
        }
        
        var savedSortAscending = settings.value("sortAscending", true)
        if (savedSortAscending !== undefined && savedSortAscending !== null) {
            sortAscending = (savedSortAscending === true || savedSortAscending === "true" || savedSortAscending === 1)
        }
        
        console.log("AppSettings loaded: theme=" + isDarkTheme + ", lang=" + language + ", rememberLogin=" + rememberLogin + ", filterMonth=" + filterMonth + ", sortField=" + sortField + ", sortAscending=" + sortAscending)
    }
}
