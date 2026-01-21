import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import StyleModule 1.0
import components
import core

Dialog {
    id: dialog
    modal: true
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    closePolicy: Dialog.NoAutoClose
    padding: Style.mediumSpacing

    property bool showPassword: false
    property bool showConfirmPassword: false

    signal registrationRequested(string login, string password)
    signal requestSignIn()

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 1
        border.color: Style.componentOutline
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
            width: parent.width
            spacing: Style.smallSpacing

            RowLayout {
                width: parent.width
                spacing: Style.smallSpacing

                Label {
                    text: qsTr("Password")
                    color: Style.textColor
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("Generate")
                    flat: true
                    onClicked: {
                        passwordField.text = UserModel.generatePassword()
                        confirmPasswordField.text = passwordField.text
                        passwordField.selectAll()
                    }
                }
            }

            Item {
                width: parent.width
                height: passwordField.height

                TextField {
                    id: passwordField
                    width: parent.width
                    placeholderText: qsTr("Min 12 chars: a-z, A-Z, 0-9, !@#$%^&*()")
                    echoMode: dialog.showPassword ? TextInput.Normal : TextInput.Password
                    onAccepted: confirmPasswordField.forceActiveFocus()
                    rightPadding: 40
                }

                Icon {
                    anchors.right: parent.right
                    anchors.rightMargin: Style.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    source: dialog.showPassword ? ResourceManager.icon("view","png") : ResourceManager.icon("hide","png")
                    implicitWidth: 30
                    implicitHeight: 30
                    MouseArea {
                        anchors.fill: parent
                        onClicked: dialog.showPassword = !dialog.showPassword
                    }
                }
            }

            Text {
                width: parent.width
                text: "• At least 12 characters\n• Lowercase letters (a-z)\n• Uppercase letters (A-Z)\n• Digits (0-9)\n• Special characters (!@#$%^&*())"
                color: Style.textSecondaryColor
                font.pixelSize: Style.smallFont
                lineHeight: 1.4
                horizontalAlignment: Text.AlignLeft
            }
        }

        Column {
            Layout.fillWidth: true
            width: parent.width
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Confirm Password")
                color: Style.textColor
                font.bold: true
            }

            Item {
                width: parent.width
                height: confirmPasswordField.height

                TextField {
                    id: confirmPasswordField
                    width: parent.width
                    placeholderText: qsTr("Confirm password")
                    echoMode: dialog.showConfirmPassword ? TextInput.Normal : TextInput.Password
                    onAccepted: registerButton.clicked()
                    rightPadding: 40
                }

                Icon {
                    anchors.right: parent.right
                    anchors.rightMargin: Style.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    source: dialog.showConfirmPassword ? ResourceManager.icon("view","png") : ResourceManager.icon("hide","png")
                    implicitWidth: 30
                    implicitHeight: 30
                    MouseArea {
                        anchors.fill: parent
                        onClicked: dialog.showConfirmPassword = !dialog.showConfirmPassword
                    }
                }
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
                    dialog.close()
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

                    if (passwordField.text !== confirmPasswordField.text) {
                        errorLabel.text = qsTr("Passwords do not match")
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
