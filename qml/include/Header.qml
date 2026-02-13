import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import QtQuick.Effects
import core
import components
import dialogs 1.0

Rectangle {
    id: root
    width: parent.width
    height: 86
    color: Style.surfaceColor
    
    property bool isSyncing: false
    property string networkErrorMessage: ""
    property bool isSessionExpired: false
    property bool serverHealthy: false
    property string serverHealthMessage: ""
    property string lastSyncAction: ""
    property string pendingProfileLogin: ""
    property string pendingProfilePassword: ""
    property bool isLanguageChanging: false

    function restoreComboBoxIndices() {
        // After retranslate, QML engine may have reset ComboBox currentIndex
        // Force restore them from AppSettings
        
        if (typeof monthFilter !== 'undefined' && monthFilter !== null) {
            var newIndex = AppSettings.filterMonth >= 0 ? AppSettings.filterMonth + 1 : 0
            monthFilter.currentIndex = newIndex
        }
        if (typeof sortBy !== 'undefined' && sortBy !== null) {
            var maxIndex = sortBy.model.length - 1
            var savedIndex = AppSettings.sortField
            var newSortIndex = Math.min(savedIndex, maxIndex)
            sortBy.currentIndex = newSortIndex
        }
        if (typeof sortRow !== 'undefined' && sortRow !== null) {
            sortRow.sort = AppSettings.sortAscending
        }
    }

    function applyFiltersAndSort() {
        // Apply saved filters and sort settings
        
        if (AppSettings.filterMonth >= 0) {
            var today = new Date()
            var selectedMonth = AppSettings.filterMonth
            var selectedYear = today.getFullYear()
            
            if (selectedMonth > today.getMonth()) {
                selectedYear = today.getFullYear() - 1
            }
            
            TaskModel.setFilterMonth(selectedMonth, selectedYear)
        } else {
            TaskModel.setFilterMonth(-1, -1)
        }
        
        TaskModel.sortField = AppSettings.sortField
        TaskModel.sortAscending = AppSettings.sortAscending
    }

    Component.onCompleted: {
        // Apply saved filters and sort settings on Header initialization
        applyFiltersAndSort()
        
        // Delay to ensure ComboBox'es are fully initialized
        Qt.callLater(function() {
            restoreComboBoxIndices()
        })
    }

    Connections {
        target: TaskModel
        
        function onShowDeletedChanged() {
            // When showDeleted toggles, restore sort settings
            applyFiltersAndSort()
        }
    }

    Connections {
        target: AppSettings
        
        function onFilterMonthChanged() {
            if (AppSettings.filterMonth >= 0) {
                var today = new Date()
                var selectedMonth = AppSettings.filterMonth
                var selectedYear = today.getFullYear()
                
                if (selectedMonth > today.getMonth()) {
                    selectedYear = today.getFullYear() - 1
                }
                
                TaskModel.setFilterMonth(selectedMonth, selectedYear)
            } else {
                TaskModel.setFilterMonth(-1, -1)
            }
        }
        
        function onSortFieldChanged() {
            TaskModel.sortField = AppSettings.sortField
        }
        
        function onSortAscendingChanged() {
            TaskModel.sortAscending = AppSettings.sortAscending
        }
    }

    Connections {
        target: LanguageManager
        
        function onLanguageChanged() {
            // Block all filter/sort updates during language change
            root.isLanguageChanging = true
            
            // After language change and retranslate, we need to restore filter/sort values
            // Use multiple Qt.callLater to ensure new elements are fully created and initialized
            Qt.callLater(function() {
                restoreComboBoxIndices()
                
                Qt.callLater(function() {
                    root.isLanguageChanging = false
                })
            })
        }
    }

    Timer {
        interval: 30000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: NetworkClient.checkHealth()
    }

    NetworkErrorDialog {
        id: networkErrorDialog
        message: networkErrorMessage
        sessionExpired: isSessionExpired
        onSignInRequested: signInDialog.open()
        onClosed: {
            isSyncing = false
        }
    }

    SyncConflictDialog {
        id: syncConflictDialog
        onResolveRequested: function(ids, useRemote) {
            TaskModel.resolveConflicts(ids, useRemote)
        }
    }

    SyncSuccessDialog {
        id: syncSuccessDialog
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
                currentIndex: AppSettings.filterMonth >= 0 ? AppSettings.filterMonth + 1 : 0
                
                property bool isInitializing: true
                
                Component.onCompleted: {
                    // Ensure currentIndex reflects AppSettings on startup
                    var expectedIndex = AppSettings.filterMonth >= 0 ? AppSettings.filterMonth + 1 : 0
                    if (currentIndex !== expectedIndex) {
                        currentIndex = expectedIndex
                    }
                    isInitializing = false
                }
                
                // Update when AppSettings changes externally
                Connections {
                    target: AppSettings
                    function onFilterMonthChanged() {
                        var newIndex = AppSettings.filterMonth >= 0 ? AppSettings.filterMonth + 1 : 0
                        monthFilter.currentIndex = newIndex
                    }
                }
                
                onActivated: function(index) {
                    // Skip update during initialization or language change
                    if (isInitializing || root.isLanguageChanging) {
                        return
                    }

                    // Only update AppSettings if the value would actually change
                    if (index === 0) {
                        if (AppSettings.filterMonth !== -1) {
                            AppSettings.filterMonth = -1
                        }
                    } else if (index > 0) {
                        var newMonth = index - 1
                        if (AppSettings.filterMonth !== newMonth) {
                            AppSettings.filterMonth = newMonth
                        }
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
                property bool sort: AppSettings.sortAscending
                property string sortImg: sort ? "arrow-down" : "arrow-up"
                
                // Watch AppSettings.sortAscending and update sort property
                Binding {
                    target: sortRow
                    property: "sort"
                    value: AppSettings.sortAscending
                }

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
                        currentIndex: {
                            // Ensure currentIndex doesn't exceed available items
                            var savedIndex = AppSettings.sortField
                            var maxIndex = model.length - 1
                            return Math.min(savedIndex, maxIndex)
                        }
                        
                        // Watch AppSettings.sortField and update currentIndex
                        Binding {
                            target: sortBy
                            property: "currentIndex"
                            value: {
                                var savedIndex = AppSettings.sortField
                                var maxIndex = sortBy.model.length - 1
                                return Math.min(savedIndex, maxIndex)
                            }
                            when: !sortBy.activeFocus
                        }
                        
                        onActivated: function(index) {
                            AppSettings.sortField = index
                            TaskModel.sortField = index
                        }
                    }
                }
                ImgButton {
                    imgScale: 1.3
                    source: ResourceManager.icon(sortRow.sortImg, "png")
                    tooltip: sortRow.sort ? qsTr("Sort ascending") : qsTr("Sort descending")
                    width: sortBy.height
                    height: sortBy.height
                    anchors.bottom: parent.bottom
                    onClicked: {
                        sortRow.sort = !sortRow.sort
                        AppSettings.sortAscending = sortRow.sort
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
                tooltip: qsTr("Reload local DB")
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
                    tooltip: qsTr("Download from server")
                    enabled: !isSyncing
                    onClicked: {
                        if (UserModel.currentUserId > 0) {
                            lastSyncAction = "download"
                            NetworkClient.downloadChangesForUser(UserModel.currentUserId)
                        } else {
                            console.log("No user logged in")
                        }
                    }
                }

                ImgButton {
                    imgScale: 1.0
                    source: ResourceManager.icon("upload", "png")
                    tooltip: qsTr("Upload to server")
                    enabled: !isSyncing
                    onClicked: {
                        if (UserModel.currentUserId > 0) {
                            lastSyncAction = "upload"
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
                    console.log("showDeleted toggled to: " + checked)
                    TaskModel.showDeleted = checked
                    // Restore filters and sort after toggling deleted items view
                    console.log("Calling applyFiltersAndSort after showDeleted toggle")
                    applyFiltersAndSort()
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
                    pendingProfileLogin = newLogin
                    pendingProfilePassword = newPassword
                    userEditDialog.showError("")
                    UserModel.updateUserWithServer(newLogin, oldPassword, newPassword)
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

            Connections {
                target: UserModel

                function onServerUserUpdateSucceeded(login) {
                    if (AppSettings.rememberLogin) {
                        AppSettings.savedLogin = login
                        if (pendingProfilePassword !== "") {
                            AppSettings.savedPasswordHash = UserModel.hashPassword(pendingProfilePassword)
                        }
                    }

                    pendingProfileLogin = ""
                    pendingProfilePassword = ""
                    userEditDialog.close()
                }

                function onServerUserUpdateFailed(error) {
                    pendingProfilePassword = ""
                    if (error.indexOf("password") !== -1 && error.indexOf("incorrect") !== -1) {
                        userEditDialog.showPasswordError(error)
                    } else {
                        userEditDialog.showError(error)
                    }
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
                tooltip: qsTr("Settings")
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
                tooltip: qsTr("Add new task")
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
            if (lastSyncAction === "download") {
                syncSuccessDialog.dialogTitle = qsTr("Download complete")
                syncSuccessDialog.message = qsTr("Data successfully received from server.")
                syncSuccessDialog.open()
            } else if (lastSyncAction === "upload") {
                syncSuccessDialog.dialogTitle = qsTr("Upload complete")
                syncSuccessDialog.message = qsTr("Data successfully sent to server.")
                syncSuccessDialog.open()
            }
            lastSyncAction = ""
        }
        
        function onSyncFailed(error) {
            isSyncing = false
            console.log("Sync failed:", error)
            lastSyncAction = ""
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
