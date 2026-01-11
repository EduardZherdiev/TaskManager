import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components

Dialog {
    id: dialog
    modal: true
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose
    padding: Style.mediumSpacing

    signal registrationRequested(string login, string password)
    signal requestSignIn()

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
            text: qsTr("User Registration")
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
                onAccepted: confirmPasswordField.forceActiveFocus()
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Confirm Password")
                color: Style.textColor
                font.bold: true
            }

            TextField {
                id: confirmPasswordField
                width: parent.width
                placeholderText: qsTr("Confirm password")
                echoMode: TextInput.Password
                onAccepted: registerButton.clicked()
            }
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
                text: qsTr("Have account? Sign in")
                onClicked: {
                    dialog.requestSignIn()
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: registerButton
                text: qsTr("Register")
                type: 2
                onClicked: {
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

                    if (passwordField.text !== confirmPasswordField.text) {
                        errorLabel.text = qsTr("Passwords do not match")
                        errorLabel.visible = true
                        return
                    }

                    if (passwordField.text.length < 4) {
                        errorLabel.text = qsTr("Password must be at least 4 characters")
                        errorLabel.visible = true
                        return
                    }

                    dialog.registrationRequested(loginField.text.trim(), passwordField.text)
                }
            }
        }
    }

    onOpened: {
        loginField.text = ""
        passwordField.text = ""
        confirmPasswordField.text = ""
        errorLabel.visible = false
        loginField.forceActiveFocus()
    }

    function showError(msg) {
        errorLabel.text = msg
        errorLabel.visible = msg && msg.length > 0
    }
}
