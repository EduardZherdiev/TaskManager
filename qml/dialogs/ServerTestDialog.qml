import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11
import QtQuick.Window 2.11
import StyleModule 1.0
import core
import components

Dialog {
    id: serverTestDialog
    title: "Server Test Console"
    width: 700
    height: 600
    modal: false
    
    // Enable moving and resizing
    property bool isMoving: false
    property real dragStartX: 0
    property real dragStartY: 0
    
    // Custom title bar for moving
    header: Rectangle {
        color: "#1976D2"
        height: 40
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 10
            
            Text {
                text: serverTestDialog.title
                color: "white"
                font.bold: true
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
            }

        }
        
        // Mouse area for dragging
        MouseArea {
            anchors.fill: parent
            anchors.rightMargin: 40
            cursorShape: Qt.OpenHandCursor
            
            onPressed: {
                isMoving = true
                dragStartX = mouseX
                dragStartY = mouseY
            }
            
            onReleased: {
                isMoving = false
            }
            
            onMouseXChanged: {
                if (isMoving && serverTestDialog.parent) {
                    serverTestDialog.x += (mouseX - dragStartX)
                }
            }
            
            onMouseYChanged: {
                if (isMoving && serverTestDialog.parent) {
                    serverTestDialog.y += (mouseY - dragStartY)
                }
            }
        }
    }
    
    Connections {
        target: NetworkClient
        
        function onHealthChecked(isHealthy, message) {
            addLog("Health Check: " + (isHealthy ? "✓ OK" : "✗ Failed") + " - " + message, isHealthy)
        }
        
        function onUserRegistered(userId) {
            addLog("✓ User Registered: ID = " + userId, true)
        }
        
        function onUserLoggedIn(userId, login) {
            addLog("✓ User Logged In: ID = " + userId + ", Login = " + login, true)
        }
        
        function onUsersReceived(users) {
            addLog("✓ Users Retrieved: " + users.length + " users", true)
            addLog("Data: " + JSON.stringify(users, null, 2), true)
        }
        
        function onTasksReceived(tasks) {
            addLog("✓ Tasks Retrieved: " + tasks.length + " tasks", true)
            addLog("Data: " + JSON.stringify(tasks, null, 2), true)
        }
        
        function onFeedbacksReceived(feedbacks) {
            addLog("✓ Feedbacks Retrieved: " + feedbacks.length + " feedbacks", true)
            addLog("Data: " + JSON.stringify(feedbacks, null, 2), true)
        }
        
        function onError(message) {
            if (message && message.length > 0) {
                addLog("✗ Error: " + message, false)
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // ===== URL SETTING =====
        RowLayout {
            spacing: 10
            
            Label {
                text: "Server URL:"
                color: Style.textColor
            }
            
            TextField {
                id: urlField
                Layout.fillWidth: true
                text: "http://127.0.0.1:8000"
                color: Style.textColor
                background: Rectangle {
                    color: Style.backgroundColor
                    border.color: Style.controlBorder
                    border.width: 1
                    radius: 4
                }
            }
            
            Button {
                text: "Set"
                onClicked: {
                    NetworkClient.setBaseUrl(urlField.text)
                    addLog("Server URL set to: " + urlField.text, true)
                }
                background: Rectangle {
                    color: "#2196F3"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        // ===== TEST BUTTONS =====
        RowLayout {
            spacing: 10
            Layout.fillWidth: true
            
            Button {
                text: "Health"
                Layout.fillWidth: true
                onClicked: {
                    addLog("Sending health check request...", false)
                    NetworkClient.checkHealth()
                }
                background: Rectangle {
                    color: "#4CAF50"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: "Get Users"
                Layout.fillWidth: true
                onClicked: {
                    addLog("Fetching users from server...", false)
                    NetworkClient.getUsers()
                }
                background: Rectangle {
                    color: "#2196F3"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: "Get Tasks"
                Layout.fillWidth: true
                onClicked: {
                    addLog("Fetching tasks from server...", false)
                    NetworkClient.getTasks()
                }
                background: Rectangle {
                    color: "#9C27B0"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: "Get Feedbacks"
                Layout.fillWidth: true
                onClicked: {
                    addLog("Fetching feedbacks from server...", false)
                    NetworkClient.getFeedbacks()
                }
                background: Rectangle {
                    color: "#FF9800"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        // ===== REGISTER/LOGIN SECTION =====
        RowLayout {
            spacing: 10
            Layout.fillWidth: true
            
            TextField {
                id: testLogin
                Layout.fillWidth: true
                placeholderText: "Login"
                color: Style.textColor
                background: Rectangle {
                    color: Style.backgroundColor
                    border.color: Style.controlBorder
                    border.width: 1
                    radius: 4
                }
            }
            
            TextField {
                id: testPassword
                Layout.fillWidth: true
                placeholderText: "Password"
                echoMode: TextInput.Password
                color: Style.textColor
                background: Rectangle {
                    color: Style.backgroundColor
                    border.color: Style.controlBorder
                    border.width: 1
                    radius: 4
                }
            }
            
            Button {
                text: "Register"
                onClicked: {
                    if (testLogin.text && testPassword.text) {
                        addLog("Registering user: " + testLogin.text, false)
                        NetworkClient.registerUser(testLogin.text, testPassword.text)
                    }
                }
                background: Rectangle {
                    color: "#FF9800"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: "Login"
                onClicked: {
                    if (testLogin.text && testPassword.text) {
                        addLog("Logging in user: " + testLogin.text, false)
                        NetworkClient.loginUser(testLogin.text, testPassword.text)
                    }
                }
                background: Rectangle {
                    color: "#2196F3"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        // ===== LOG OUTPUT =====
        Label {
            text: "Console Output:"
            color: Style.textColor
            Layout.fillWidth: true
        }
        
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Style.backgroundColor
            border.color: Style.controlBorder
            border.width: 1
            radius: 4
            
            ScrollView {
                anchors.fill: parent
                clip: true
                
                TextEdit {
                    id: logOutput
                    width: parent.width
                    color: Style.textColor
                    readOnly: true
                    selectByMouse: true
                    font.pixelSize: 11
                    font.family: "Courier New"
                    padding: 10
                    wrapMode: TextEdit.Wrap
                    text: "Server test console ready...\nPassword requirements: min 12 chars, uppercase, lowercase, digits, special chars (!@#$%^&*())\n"
                }
            }
        }
        
        // ===== BUTTONS =====
        RowLayout {
            spacing: 10
            Layout.fillWidth: true
            
            Button {
                text: "Clear Log"
                Layout.fillWidth: true
                onClicked: {
                    addLog("Log cleared", true)
                    logOutput.text = ""
                }
                background: Rectangle {
                    color: "#607D8B"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: "Close"
                Layout.fillWidth: true
                onClicked: serverTestDialog.close()
                background: Rectangle {
                    color: "#f44336"
                    radius: 4
                }
                contentItem: Text {
                    color: Style.textColor
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
    
    function addLog(message, success) {
        var timestamp = new Date().toLocaleTimeString(Qt.locale(), "hh:mm:ss")
        var prefix = success ? "[✓]" : "[✗]"
        
        // Разбить многострочные сообщения
        var lines = message.split("\n")
        for (var i = 0; i < lines.length; i++) {
            if (i === 0) {
                logOutput.text += timestamp + " " + prefix + " " + lines[i] + "\n"
            } else if (lines[i].length > 0) {
                logOutput.text += "         " + lines[i] + "\n"
            }
        }
    }
}
