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

    property int userId: -1
    property string currentLogin: ""
    property bool passwordChangeVisible: false

    signal updateRequested(int userId, string newLogin, string oldPassword, string newPassword)
    signal signOutRequested()

    background: Rectangle {
        radius: Style.largeRadius
        color: Style.surfaceColor
        border.width: 1
        border.color: Style.componentOutline
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.mediumSpacing

        Icon {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 100
            implicitHeight: 100
            source: ResourceManager.icon("user", "png")
            invertColors: true
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Edit Profile")
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
                placeholderText: qsTr("Enter new login")
                text: dialog.currentLogin
            }
        }

        Button {
            text: qsTr("Change Password")
            Layout.fillWidth: true
            visible: !passwordChangeVisible
            onClicked: {
                passwordChangeVisible = true
                currentPasswordField.forceActiveFocus()
            }
        }

        Column {
            id: passwordChangeBlock
            Layout.fillWidth: true
            spacing: Style.mediumSpacing
            visible: passwordChangeVisible

            property bool showCurrentPassword: false
            property bool showNewPassword: false
            property bool showConfirmPassword: false

            Column {
                Layout.fillWidth: true
                spacing: Style.smallSpacing
                width: parent.width

                Label {
                    text: qsTr("Current Password")
                    color: Style.textColor
                    font.bold: true
                }

                Item {
                    width: parent.width
                    height: currentPasswordField.height

                    TextField {
                        id: currentPasswordField
                        width: parent.width
                        placeholderText: qsTr("Enter current password")
                        echoMode: passwordChangeBlock.showCurrentPassword ? TextInput.Normal : TextInput.Password
                        onAccepted: newPasswordField.forceActiveFocus()
                        rightPadding: 40
                    }

                    Icon {
                        anchors.right: parent.right
                        anchors.rightMargin: Style.smallSpacing
                        anchors.verticalCenter: parent.verticalCenter
                        source: passwordChangeBlock.showCurrentPassword ? ResourceManager.icon("view","png") : ResourceManager.icon("hide","png")
                        implicitWidth: 30
                        implicitHeight: 30
                        MouseArea {
                            anchors.fill: parent
                            onClicked: passwordChangeBlock.showCurrentPassword = !passwordChangeBlock.showCurrentPassword
                        }
                    }
                }

                Label {
                    id: currentPasswordErrorLabel
                    width: parent.width
                    color: Style.errorColor
                    font.pixelSize: Style.smallFont
                    visible: false
                    wrapMode: Text.Wrap
                }
            }

            Column {
                Layout.fillWidth: true
                spacing: Style.smallSpacing
                width: parent.width

                Label {
                    text: qsTr("New Password")
                    color: Style.textColor
                    font.bold: true
                }

                Item {
                    width: parent.width
                    height: newPasswordField.height

                    TextField {
                        id: newPasswordField
                        width: parent.width
                        placeholderText: qsTr("Enter new password")
                        echoMode: passwordChangeBlock.showNewPassword ? TextInput.Normal : TextInput.Password
                        onAccepted: confirmPasswordField.forceActiveFocus()
                        rightPadding: 40
                    }

                    Icon {
                        anchors.right: parent.right
                        anchors.rightMargin: Style.smallSpacing
                        anchors.verticalCenter: parent.verticalCenter
                        source: passwordChangeBlock.showNewPassword ? ResourceManager.icon("view","png") : ResourceManager.icon("hide","png")
                        implicitWidth: 30
                        implicitHeight: 30
                        MouseArea {
                            anchors.fill: parent
                            onClicked: passwordChangeBlock.showNewPassword = !passwordChangeBlock.showNewPassword
                        }
                    }
                }
            }

            Column {
                Layout.fillWidth: true
                spacing: Style.smallSpacing
                width: parent.width

                Label {
                    text: qsTr("Confirm New Password")
                    color: Style.textColor
                    font.bold: true
                }

                Item {
                    width: parent.width
                    height: confirmPasswordField.height

                    TextField {
                        id: confirmPasswordField
                        width: parent.width
                        placeholderText: qsTr("Confirm new password")
                        echoMode: passwordChangeBlock.showConfirmPassword ? TextInput.Normal : TextInput.Password
                        onAccepted: saveButton.clicked()
                        rightPadding: 40
                    }

                    Icon {
                        anchors.right: parent.right
                        anchors.rightMargin: Style.smallSpacing
                        anchors.verticalCenter: parent.verticalCenter
                        source: passwordChangeBlock.showConfirmPassword ? ResourceManager.icon("view","png") : ResourceManager.icon("hide","png")
                        implicitWidth: 30
                        implicitHeight: 30
                        MouseArea {
                            anchors.fill: parent
                            onClicked: passwordChangeBlock.showConfirmPassword = !passwordChangeBlock.showConfirmPassword
                        }
                    }
                }
            }

            Button {
                text: qsTr("Cancel Password Change")
                type: 1
                width: parent.width
                onClicked: {
                    passwordChangeVisible = false
                    currentPasswordField.text = ""
                    newPasswordField.text = ""
                    confirmPasswordField.text = ""
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
                text: qsTr("Close")
                onClicked: dialog.close()
            }

            Item { Layout.fillWidth: true }

            Button {
                id: saveButton
                text: qsTr("Save")
                type: 2
                onClicked: {
                    errorLabel.visible = false

                    if (loginField.text.trim() === "") {
                        errorLabel.text = qsTr("Login cannot be empty")
                        errorLabel.visible = true
                        return
                    }

                    // If password change is active, validate the fields
                    if (passwordChangeVisible) {
                        currentPasswordErrorLabel.visible = false
                        
                        if (currentPasswordField.text === "") {
                            currentPasswordErrorLabel.text = qsTr("Please enter current password")
                            currentPasswordErrorLabel.visible = true
                            return
                        }

                        // Check if current password is correct before proceeding
                        if (!UserModel.checkPassword(currentPasswordField.text)) {
                            currentPasswordErrorLabel.text = UserModel.lastError
                            currentPasswordErrorLabel.visible = true
                            return
                        }

                        if (newPasswordField.text === "") {
                            errorLabel.text = qsTr("Please enter new password")
                            errorLabel.visible = true
                            return
                        }

                        if (newPasswordField.text !== confirmPasswordField.text) {
                            errorLabel.text = qsTr("New passwords do not match")
                            errorLabel.visible = true
                            return
                        }

                        if (newPasswordField.text.length < 4) {
                            errorLabel.text = qsTr("New password must be at least 4 characters")
                            errorLabel.visible = true
                            return
                        }
                    }

                    // Send password only if changing
                    var newPassword = passwordChangeVisible && newPasswordField.text !== "" ? newPasswordField.text : ""
                    var oldPassword = passwordChangeVisible ? currentPasswordField.text : ""
                    dialog.updateRequested(dialog.userId, loginField.text.trim(), oldPassword, newPassword)
                }
            }
        }
    }

    function openForUser(id, login) {
        userId = id
        currentLogin = login
        loginField.text = login
        currentPasswordField.text = ""
        newPasswordField.text = ""
        confirmPasswordField.text = ""
        passwordChangeVisible = false
        errorLabel.visible = false
        open()
    }

    onOpened: {
        loginField.forceActiveFocus()
    }

    function showError(msg) {
        // Check if error is about current password
        if (msg.indexOf("password") !== -1 && msg.indexOf("incorrect") !== -1) {
            currentPasswordErrorLabel.text = msg
            currentPasswordErrorLabel.visible = true
            errorLabel.visible = false
        } else {
            errorLabel.text = msg
            errorLabel.visible = msg && msg.length > 0
            currentPasswordErrorLabel.visible = false
        }
    }

    function showPasswordError(msg) {
        currentPasswordErrorLabel.text = msg
        currentPasswordErrorLabel.visible = true
        errorLabel.visible = false
    }
}
