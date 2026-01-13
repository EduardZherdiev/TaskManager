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
                    "January", "February", "March", "April", "May", "June",
                    "July", "August", "September", "October", "November", "December"
                ]
                currentIndex: new Date().getMonth()
                onActivated: function(index) {
                    var today = new Date()
                    var selectedMonth = index
                    var selectedYear = today.getFullYear()
                    
                    // If selected month is in the future, it's from previous year
                    if (selectedMonth > today.getMonth()) {
                        selectedYear = today.getFullYear() - 1
                    }
                    
                    TaskModel.setFilterMonth(selectedMonth, selectedYear)
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
                            ? ["Created at", "Title", "State", "Updated at", "Deleted at"]
                            : ["Created at", "Title", "State", "Updated at"]
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

        // ===== SWITCH =====
        Column {
            spacing: Style.smallSpacing
            anchors.bottom: parent.bottom

            Icon {
                width: 26
                height: 26
                anchors.horizontalCenter: parent.horizontalCenter
                source: ResourceManager.icon("delete", "png")
                canChange: false
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
                    
                    // Clear saved credentials if not remembering
                    if (!AppSettings.rememberLogin) {
                        AppSettings.savedLogin = ""
                        AppSettings.savedPasswordHash = ""
                    }
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
                    
                    // TaskModel will automatically update when UserModel.currentUserChanged signal is emitted
                    
                    // Save credentials if remember is enabled
                    if (AppSettings.rememberLogin) {
                        AppSettings.savedLogin = login
                        AppSettings.savedPasswordHash = UserModel.hashPassword(password)
                    } else {
                        AppSettings.savedLogin = ""
                        AppSettings.savedPasswordHash = ""
                    }
                    
                    close()
                }
                onRequestRegister: {
                    registrationDialog.open()
                }
            }

            UserRegistrationDialog {
                id: registrationDialog
                onRegistrationRequested: function(login, password) {
                    if (!UserModel.registerUser(login, password)) {
                        showError(UserModel.lastError)
                        return
                    }
                    
                    // TaskModel will automatically update when UserModel.currentUserChanged signal is emitted
                    
                    // Save credentials if remember is enabled
                    if (AppSettings.rememberLogin) {
                        AppSettings.savedLogin = login
                        AppSettings.savedPasswordHash = UserModel.hashPassword(password)
                    }
                    
                    close()
                }
                onRequestSignIn: {
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
}
