pragma Singleton

import QtQuick 2.0

QtObject {
    /* ====== THEME ====== */
    property bool isDarkTheme: true

    /* ====== BASE COLORS ====== */
    readonly property color primaryColor: isDarkTheme ? "#6200EE" : "#BB86FC"
    readonly property color primaryVariantColor: "#3700B3"
    readonly property color secondaryColor: "#03DAC6"
    readonly property color errorColor: isDarkTheme ? "#c92222" : "#E57373"

    readonly property color backgroundColor: isDarkTheme ? "#121212" : "#FFFFFF"
    readonly property color surfaceColor: isDarkTheme ? "#1E1E1E" : "#F7F7F7"

    readonly property color textColor: isDarkTheme ? "#FFFFFF" : "#000000"
    readonly property color textSecondaryColor: isDarkTheme ? "#B0B0B0" : "#555555"

    /* ====== OPACITY ====== */
    readonly property real defaultOpacity: 1.0
    readonly property real emphasisOpacity: 0.87
    readonly property real secondaryOpacity: 0.6
    readonly property real disabledOpacity: 0.38

    /* ====== COMPONENTS COLORS ====== */
    readonly property color componentBackground: isDarkTheme ? "#666464" : "#E0E0E0"
    readonly property color componentHover:      isDarkTheme ? "#757373" : "#D5D5D5"
    readonly property color componentPressed:    isDarkTheme ? "#5e5d5d" : "#C8C8C8"

    readonly property color componentText:        isDarkTheme ? "#FFFFFF" : "#000000"
    readonly property color componentDisabled:    isDarkTheme ? "#2A2A2A" : "#EEEEEE"
    readonly property color componentDisabledText:isDarkTheme ? "#777777" : "#9E9E9E"

    readonly property color componentOutline: isDarkTheme ? "#FFFFFF22" : "#00000022"
    readonly property color componentFocusRing: secondaryColor

    /* ====== PRIMARY BUTTON ====== */
    readonly property color primaryHover: isDarkTheme ? "#7E3FF2" : "#C9A7FF"
    readonly property color primaryPressed: isDarkTheme ? "#4A00C2" : "#9A67EA"

    /* ====== ERROR BUTTON ====== */
    readonly property color errorHover: isDarkTheme ? "#d62929" : "#CF6679"
    readonly property color errorPressed: isDarkTheme ? "#e62727" : "#D32F2F"

    /* ====== TASK STATUS COLORS ====== */
    readonly property color statusCompleted: "#4CAF50"
    readonly property color statusInProgress: "#FFC107"
    readonly property color statusArchived: "#F44336"

    /* ====== INPUT / CONTROL ====== */
    readonly property color controlBackground: surfaceColor
    readonly property color controlBorder: isDarkTheme ? "#444444" : "#CCCCCC"
    readonly property color controlActiveBorder: primaryColor
    readonly property color controlErrorBorder: errorColor

    /* ====== SPACING ====== */
    readonly property int bigSpacing: 50
    readonly property int mediumSpacing: 15
    readonly property int smallSpacing: 5

    readonly property int defaultOffset: 15
    readonly property int mediumOffset: 10
    readonly property int tinyOffset: 5

    /* ====== RADIUS ====== */
    readonly property int smallRadius: 4
    readonly property int mediumRadius: 8
    readonly property int largeRadius: 12

    /* ====== ANIMATION ====== */
    readonly property int fastAnimation: 100
    readonly property int normalAnimation: 200
    readonly property int slowAnimation: 300

    /* ====== FONT SIZE ====== */
    readonly property int smallFont: 12
    readonly property int mediumFont: 20
    readonly property int largeFont: 40
}
