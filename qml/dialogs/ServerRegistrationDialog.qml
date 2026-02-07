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
    width: 520

    property bool showPassword: false
    property bool showConfirmPassword: false
    property bool isLoading: false

    signal registrationStarted()
    signal registrationSucceeded()
    signal registrationFailed(string error)

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
            text: qsTr("Server Registration")
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
                enabled: !dialog.isLoading
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

            Item {
                width: parent.width
                height: passwordField.height

                TextField {
                    id: passwordField
                    width: parent.width
                    placeholderText: qsTr("Min 12 chars: a-z, A-Z, 0-9, !@#$%^&*()")
                    echoMode: dialog.showPassword ? TextInput.Normal : TextInput.Password
                    enabled: !dialog.isLoading
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
        }

        Column {
            Layout.fillWidth: true
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
                    enabled: !dialog.isLoading
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

        ScrollView {
            id: errorScroll
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            visible: errorText.text.length > 0
            clip: true

            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            Text {
                id: errorText
                width: parent.width
                color: Style.errorColor
                font.pixelSize: Style.smallFont
                wrapMode: Text.Wrap
            }
        }

        BusyIndicator {
            id: loadingIndicator
            Layout.alignment: Qt.AlignHCenter
            visible: dialog.isLoading
            running: visible
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.mediumSpacing

            Button {
                text: qsTr("Cancel")
                enabled: !dialog.isLoading
                onClicked: {
                    dialog.close()
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: registerButton
                text: qsTr("Register")
                type: 2
                enabled: !dialog.isLoading
                onClicked: {
                    if (loginField.text.trim() === "") {
                        errorText.text = qsTr("Login cannot be empty")
                        return
                    }

                    if (passwordField.text !== confirmPasswordField.text) {
                        errorText.text = qsTr("Passwords do not match")
                        return
                    }

                    dialog.isLoading = true
                    dialog.registrationStarted()
                    UserModel.registerUserWithServer(loginField.text.trim(), passwordField.text)
                }
            }
        }
    }

    onOpened: {
        loginField.text = ""
        passwordField.text = ""
        confirmPasswordField.text = ""
        errorText.text = ""
        dialog.isLoading = false
        loginField.forceActiveFocus()
    }

    function showError(msg) {
        errorText.text = msg
        dialog.isLoading = false
    }

    function showSuccess() {
        dialog.isLoading = false
        dialog.close()
    }
}
