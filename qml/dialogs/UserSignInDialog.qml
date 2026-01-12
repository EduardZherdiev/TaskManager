import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import core
import components

Dialog {
    id: dialog
    modal: true
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose

    padding: Style.mediumSpacing

    signal signInRequested(string login, string password)
    signal requestRegister()

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 0
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        Label {
            Layout.fillWidth: true
            text: qsTr("Sign In")
            font.pixelSize: Style.largeFont
            font.bold: true
            color: Style.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Style.componentOutline
        }

        Column {
            Layout.fillWidth: true
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Login")
                color: Style.textColor
                font.bold: true
            }

            TextField {
                id: loginField
                width: parent.width
                placeholderText: qsTr("Enter login")
                onAccepted: passwordField.forceActiveFocus()
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Password")
                color: Style.textColor
                font.bold: true
            }

            TextField {
                id: passwordField
                width: parent.width
                placeholderText: qsTr("Enter password")
                echoMode: TextInput.Password
                onAccepted: signInButton.clicked()
            }
        }

        CheckBox {
            Layout.fillWidth: true
            text: qsTr("Remember me")
            checked: AppSettings.rememberLogin
            onCheckedChanged: AppSettings.rememberLogin = checked
        }

        Label {
            id: errorLabel
            Layout.fillWidth: true
            color: Style.errorColor
            font.pixelSize: Style.smallFont
            visible: false
            wrapMode: Text.Wrap
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.mediumSpacing

            Button {
                text: qsTr("New Account?")
                onClicked: {
                    dialog.requestRegister()
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: signInButton
                text: qsTr("Sign In")
                type: 2
                onClicked: {
                    console.log("Sign In button clicked")
                    errorLabel.visible = false

                    if (loginField.text.trim() === "") {
                        errorLabel.text = qsTr("Login cannot be empty")
                        errorLabel.visible = true
                        return
                    }

                    if (passwordField.text === "") {
                        errorLabel.text = qsTr("Password cannot be empty")
                        errorLabel.visible = true
                        return
                    }

                    console.log("Emitting signInRequested signal with login=" + loginField.text.trim())
                    dialog.signInRequested(loginField.text.trim(), passwordField.text)
                }
            }
        }
    }

    onOpened: {
        loginField.text = ""
        passwordField.text = ""
        errorLabel.visible = false
        loginField.forceActiveFocus()
    }

    function showError(msg) {
        errorLabel.text = msg
        errorLabel.visible = msg && msg.length > 0
    }
}
