import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts
import StyleModule 1.0
import include
import components
import core
import dialogs 1.0
import pages

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
        if (AppSettings.rememberLogin && AppSettings.savedLogin !== "" && AppSettings.savedPasswordHash !== "") {
            // Use signInWithHash for auto-login since we already have the hash
            if (UserModel.signInWithHash(AppSettings.savedLogin, AppSettings.savedPasswordHash)) {
                // TaskModel will automatically update when UserModel.currentUserChanged signal is emitted
                return
            } else {
                console.log("Auto-login failed, clearing saved credentials")
                AppSettings.savedLogin = ""
                AppSettings.savedPasswordHash = ""
            }
        }
        
        if (UserModel.currentUserId < 0)
            header.openSignInDialog()
    }

    Rectangle {
        id: _background
        z: -100
        anchors.fill: parent
        color: Style.backgroundColor
    }

    header: Header{
        id: headerComponent
    }

    Body{}

    // UserSignInDialog is now handled in Header component
    
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
            headerComponent.openSignInDialog()
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
