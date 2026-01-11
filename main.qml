import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts
import StyleModule 1.0
import include
import components
import core
import dialogs 1.0

ApplicationWindow {

    id: root
    visible: true
    width: 1000
    height: 700
    title: qsTr("Task manager")
    property var styleWindow: null

    Component.onCompleted: {
        // Apply saved theme on startup
        Style.isDarkTheme = AppSettings.isDarkTheme
        
        // Try auto-login if remember is enabled and credentials are saved
        if (AppSettings.rememberLogin && AppSettings.savedLogin !== "" && AppSettings.savedPassword !== "") {
            if (UserModel.signIn(AppSettings.savedLogin, AppSettings.savedPassword)) {
                console.log("Auto-login successful for:", AppSettings.savedLogin)
                return
            } else {
                console.log("Auto-login failed, clearing saved credentials")
                AppSettings.savedLogin = ""
                AppSettings.savedPassword = ""
            }
        }
        
        if (UserModel.currentUserId < 0)
            signInDialog.open()
    }

    Rectangle {
        id: _background
        z: -100
        anchors.fill: parent
        color: Style.backgroundColor
    }

    header: Header{}

    Body{}

    UserSignInDialog {
        id: signInDialog
        onSignInRequested: function(login, password) {
            if (!UserModel.signIn(login, password)) {
                showError(UserModel.lastError)
                return
            }
            close()
        }
        onRequestRegister: {
            close()
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
            close()
        }
        onRequestSignIn: {
            close()
            signInDialog.open()
        }
    }

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
