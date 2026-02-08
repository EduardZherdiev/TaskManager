import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import QtQuick.Effects
import core
import components
import dialogs 1.0

Rectangle {
    width: parent.width
    height: 86
    color: Style.surfaceColor
    
    property bool isSyncing: false
    property string networkErrorMessage: ""
    property bool isSessionExpired: false
    property bool serverHealthy: false
    property string serverHealthMessage: ""

    Timer {
        interval: 30000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: NetworkClient.checkHealth()
    }

    Dialog {
        id: networkErrorDialog
        modal: true
        parent: Overlay.overlay
        anchors.centerIn: Overlay.overlay
        title: ""
        width: 420
        height: 250

        background: Rectangle {
            radius: Style.largeRadius
            color: Style.surfaceColor

            Rectangle {
                visible: Style.isDarkTheme
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }

            Rectangle {
                visible: Style.isDarkTheme
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }
        }

        onOpened: {
            errorText.text = networkErrorMessage
            errorText.focus = false
            actionButton.forceActiveFocus()
        }

        onClosed: {
            isSyncing = false
        }

        contentItem: TextEdit {
            id: errorText
            color: Style.textColor
            readOnly: true
            wrapMode: TextEdit.Wrap
            padding: 10
            selectedTextColor: Style.textColor
            selectionColor: Style.primaryColor
            selectByMouse: true
        }

        header: Rectangle {
            height: 42
            color: Style.surfaceColor
            border.color: Style.componentOutline
            border.width: Style.isDarkTheme ? 0 : 1

            Rectangle {
                visible: Style.isDarkTheme
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 1
                color: Style.componentOutline
            }

            Rectangle {
                visible: Style.isDarkTheme
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }

            Rectangle {
                visible: Style.isDarkTheme
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Style.componentOutline
            }


            Label {
                anchors.centerIn: parent
                text: isSessionExpired ? qsTr("Session expired") : qsTr("Server error")
                color: Style.textColor
                font.bold: true
            }
        }

        footer: DialogButtonBox {
            background: Rectangle {
                color: Style.surfaceColor
                border.color: Style.componentOutline
                border.width: Style.isDarkTheme ? 0 : 1

                Rectangle {
                    visible: Style.isDarkTheme
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1
                    color: Style.componentOutline
                }

                Rectangle {
                    visible: Style.isDarkTheme
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 1
                    color: Style.componentOutline
                }

                Rectangle {
                    visible: Style.isDarkTheme
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 1
                    color: Style.componentOutline
                }
            }

            Button {
                id: actionButton
                text: isSessionExpired ? qsTr("Sign in") : qsTr("OK")
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                onClicked: {
                    networkErrorDialog.close()
                    if (isSessionExpired) {
                        signInDialog.open()
                    }
                }

                Keys.onReturnPressed: {
                    networkErrorDialog.close()
                    if (isSessionExpired) {
                        signInDialog.open()
                    }
                }
                Keys.onEscapePressed: {
                    networkErrorDialog.close()
                }
            }
        }
    }

    SyncConflictDialog {
        id: syncConflictDialog
        onResolveRequested: function(ids, useRemote) {
            TaskModel.resolveConflicts(ids, useRemote)
        }
    }


    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Style.controlBorder
    }

    Row {
        anchors.centerIn: parent
        spacing: Style.bigSpacing

        // ===== MONTH FILTER =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Label {
                leftPadding: 7
                text: qsTr("Filter by month")
                color: Style.textColor
            }

            ComboBox {
                id: monthFilter
                model: [
                    qsTr("All"),
                    qsTr("January"), qsTr("February"), qsTr("March"), qsTr("April"), qsTr("May"), qsTr("June"),
                    qsTr("July"), qsTr("August"), qsTr("September"), qsTr("October"), qsTr("November"), qsTr("December")
                ]
                currentIndex: new Date().getMonth() + 1
                onActivated: function(index) {
                    if (index === 0) {
                        // "All" selected - show all months
                        TaskModel.setFilterMonth(-1, -1)
                    } else {
                        // Month selected (1-12, so actual month is index-1)
                        var today = new Date()
                        var selectedMonth = index - 1
                        var selectedYear = today.getFullYear()
                        
                        // If selected month is in the future, it's from previous year
                        if (selectedMonth > today.getMonth()) {
                            selectedYear = today.getFullYear() - 1
                        }
                        
                        TaskModel.setFilterMonth(selectedMonth, selectedYear)
                    }
                }
            }
        }

        // ===== SORTING =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Row {
                id: sortRow
                spacing: Style.smallSpacing
                property bool sort: true // desk
                property string sortImg: sort ? "arrow-down" : "arrow-up"

                Column {
                    spacing: 2

                    Label {
                        leftPadding: 7
                        text: qsTr("Sorting")
                        color: Style.textColor
                    }
                    ComboBox {
                        id: sortBy
                        model: TaskModel.showDeleted 
                            ? [qsTr("Created at"), qsTr("Title"), qsTr("State"), qsTr("Updated at"), qsTr("Deleted at")]
                            : [qsTr("Created at"), qsTr("Title"), qsTr("State"), qsTr("Updated at")]
                        currentIndex: TaskModel.sortField
                        onActivated: function(index) {
                            TaskModel.sortField = index
                        }
                    }
                }
                ImgButton {
                    width: sortBy.height
                    height: sortBy.height
                    imgScale: 1.3
                    anchors.bottom: parent.bottom
                    source: ResourceManager.icon(sortRow.sortImg, "png")
                    onClicked: {
                        sortRow.sort = !sortRow.sort
                        TaskModel.sortAscending = sortRow.sort
                    }
                }
            }
        }

        // ===== REFRESH =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            ImgButton {
                source: ResourceManager.icon("refresh", "png")
                onClicked: {
                    TaskModel.reloadTasks()
                }
            }
        }
        
        // ===== SYNC BUTTONS =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            Row {
                spacing: Style.smallSpacing

                ImgButton {
                    imgScale: 1.0
                    source: ResourceManager.icon("download", "png")
                    tooltip: qsTr("Download changes from server")
                    enabled: !isSyncing
                    onClicked: {
                        if (UserModel.currentUserId > 0) {
                            NetworkClient.downloadChangesForUser(UserModel.currentUserId)
                        } else {
                            console.log("No user logged in")
                        }
                    }
                }

                ImgButton {
                    imgScale: 1.0
                    source: ResourceManager.icon("upload", "png")
                    tooltip: qsTr("Upload changes to server")
                    enabled: !isSyncing
                    onClicked: {
                        if (UserModel.currentUserId > 0) {
                            NetworkClient.uploadChangesForUser(UserModel.currentUserId)
                        } else {
                            console.log("No user logged in")
                        }
                    }
                }
                
                BusyIndicator {
                    visible: isSyncing
                    running: isSyncing
                    scale: 0.7
                    anchors.verticalCenter: parent.verticalCenter
                    palette.text: Style.primaryColor
                    palette.highlight: Style.primaryColor
                }

                Rectangle {
                    id: healthIndicator
                    width: 10
                    height: 10
                    radius: 5
                    color: serverHealthy ? Style.statusCompleted : Style.statusArchived
                    anchors.verticalCenter: parent.verticalCenter

                    MouseArea {
                        id: healthMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (!serverHealthy) {
                                var msg = serverHealthMessage
                                if (!msg || msg.length === 0) {
                                    msg = qsTr("Server offline. Please check your connection.")
                                }
                                showNetworkError(msg)
                            }
                        }
                    }

                    ToolTip.visible: healthMouseArea.containsMouse
                    ToolTip.text: serverHealthy
                        ? qsTr("Server online")
                        : qsTr("Server offline")
                }
            }
        }

        // ===== SWITCH =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Icon {
                width: 26
                height: 26
                anchors.horizontalCenter: parent.horizontalCenter
                source: ResourceManager.icon("delete", "png")
                invertColors: false
            }

            Switch {
                bottomPadding: 0

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                checked: TaskModel.showDeleted
                onCheckedChanged: {
                    TaskModel.showDeleted = checked
                }
            }
        }

        // ===== SETTINGS ICON =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }

            SettingsDialog {
                id: settingsDialog
                onProfileEditRequested: {
                    userEditDialog.openForUser(UserModel.currentUserId, UserModel.currentUserLogin)
                    userEditDialog.open()
                }
                onSignOutRequested: {
                    // Call signOut on UserModel - TaskModel will automatically update
                    UserModel.signOut()
                    NetworkClient.clearAccessToken()
                    NetworkClient.clearRefreshToken()
                    
                    // Always clear saved credentials on sign out
                    AppSettings.rememberLogin = false
                    AppSettings.savedLogin = ""
                    AppSettings.savedPasswordHash = ""
                    AppSettings.savedAccessToken = ""
                    AppSettings.savedRefreshToken = ""
                    
                    // After sign out, open sign-in dialog
                    signInDialog.open()
                }
            }

            UserEditDialog {
                id: userEditDialog
                onUpdateRequested: function(userId, newLogin, oldPassword, newPassword) {
                    if (!UserModel.updateUser(newLogin, oldPassword, newPassword)) {
                        var errorMsg = UserModel.lastError
                        // Show password error in the specific field
                        if (errorMsg.indexOf("password") !== -1 && errorMsg.indexOf("incorrect") !== -1) {
                            userEditDialog.showPasswordError(errorMsg)
                        } else {
                            userEditDialog.showError(errorMsg)
                        }
                        return
                    }
                    
                    // Update saved credentials if remember is enabled
                    if (AppSettings.rememberLogin) {
                        AppSettings.savedLogin = newLogin
                        if (newPassword !== "") {
                            AppSettings.savedPasswordHash = UserModel.hashPassword(newPassword)
                        }
                    }
                    
                    userEditDialog.close()
                }
                onSignOutRequested: {
                    // Call signOut on UserModel
                    UserModel.signOut()
                    NetworkClient.clearAccessToken()
                    NetworkClient.clearRefreshToken()
                    // Always clear saved credentials on sign out
                    AppSettings.rememberLogin = false
                    AppSettings.savedLogin = ""
                    AppSettings.savedPasswordHash = ""
                    AppSettings.savedAccessToken = ""
                    AppSettings.savedRefreshToken = ""
                    // After sign out, open sign-in dialog
                    signInDialog.open()
                }
            }

            UserSignInDialog {
                id: signInDialog
                onSignInRequested: function(login, password) {
                    console.log("=== Header.onSignInRequested handler called ===")
                    console.log("Sign in requested: login=" + login)
                    if (!UserModel.signIn(login, password)) {
                        console.log("Sign in failed: " + UserModel.lastError)
                        showError(UserModel.lastError)
                        return
                    }

                    NetworkClient.loginUser(login, password)
                    
                    // TaskModel will automatically update when UserModel.currentUserChanged signal is emitted
                    
                    // Save credentials if remember is enabled
                    if (AppSettings.rememberLogin) {
                        AppSettings.savedLogin = login
                        AppSettings.savedPasswordHash = UserModel.hashPassword(password)
                    } else {
                        AppSettings.savedLogin = ""
                        AppSettings.savedPasswordHash = ""
                        AppSettings.savedAccessToken = ""
                        AppSettings.savedRefreshToken = ""
                    }
                    
                    close()
                }
                onRequestRegister: {
                    registrationDialog.open()
                }
            }

            UserRegistrationDialog {
                id: registrationDialog
                onRequestSignIn: {
                    close()
                    signInDialog.open()
                }
            }

            ImgButton {
                imgScale: 1.2
                source: ResourceManager.icon("settings")
                onClicked: settingsDialog.open()
            }
        }

        // ===== ADD =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom
            Item {
                height: 20
            }
            TaskDialog {
                id: addTaskDialog
                onSaveRequested: function(taskId, title, description, state) {
                    console.log("Create task request", taskId, title, description, state)
                    TaskModel.createTask(title, description, state)
                }
            }

            ImgButton {
                imgScale: 0.8
                source: ResourceManager.icon("add", "png")
                onClicked: addTaskDialog.openForTask(-1, "", "", 0, "", "", "")
            }
        }
    }

    function openSignInDialog() {
        signInDialog.open()
    }

    function showNetworkError(msg) {
        if (!msg || msg.length === 0) {
            msg = qsTr("Network error. Please check your connection.")
        }
        isSessionExpired = msg.indexOf("Session expired") !== -1
        networkErrorMessage = msg
        networkErrorDialog.open()
    }
    
    // Handle sync signals
    Connections {
        target: NetworkClient
        
        function onSyncStarted() {
            isSyncing = true
            console.log("Sync started...")
        }
        
        function onSyncCompleted(message) {
            isSyncing = false
            console.log("Sync completed:", message)
        }
        
        function onSyncFailed(error) {
            isSyncing = false
            console.log("Sync failed:", error)
            showNetworkError(error)
        }

        function onHealthChecked(isHealthy, message) {
            serverHealthy = isHealthy
            serverHealthMessage = message
        }
    }

    Connections {
        target: NetworkClient

        function onError(message) {
            if (registrationDialog.visible) {
                registrationDialog.showError(message)
                return
            }
            showNetworkError(message)
        }

        function onAccessTokenChanged(token) {
            if (AppSettings.rememberLogin) {
                AppSettings.savedAccessToken = token
            } else {
                AppSettings.savedAccessToken = ""
            }
        }

        function onRefreshTokenChanged(token) {
            if (AppSettings.rememberLogin) {
                AppSettings.savedRefreshToken = token
            } else {
                AppSettings.savedRefreshToken = ""
            }
        }
    }

    Connections {
        target: TaskModel

        function onConflictsDetected(conflicts) {
            syncConflictDialog.setConflicts(conflicts)
            if (conflicts.length > 0) {
                syncConflictDialog.open()
            } else {
                syncConflictDialog.close()
            }
        }
    }
}
