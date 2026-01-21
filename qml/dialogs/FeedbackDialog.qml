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
    closePolicy: Dialog.CloseOnEscape
    width: 500

    padding: Style.mediumSpacing

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
            text: qsTr("Your Feedback")
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
                text: qsTr("Rating")
                color: Style.textColor
                font.bold: true
            }

            Row {
                spacing: Style.smallSpacing
                
                Repeater {
                    model: 5
                    
                    Button {
                        width: 50
                        height: 50
                        text: "★"
                        font.pixelSize: 24
                        enabled: true
                        
                        background: Rectangle {
                            color: index < ratingValue ? Style.primaryColor : Style.controlBackground
                            radius: Style.mediumRadius
                            border.width: 1
                            border.color: Style.controlBorder
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: index < ratingValue ? "white" : Style.textColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        onClicked: {
                            ratingValue = index + 1
                        }
                    }
                }
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: Style.smallSpacing

            Label {
                text: qsTr("Description")
                color: Style.textColor
                font.bold: true
            }

            TextArea {
                id: descriptionField
                width: parent.width
                height: 150
                placeholderText: qsTr("Tell us about your experience...")
                wrapMode: TextEdit.Wrap
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

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            Button {
                text: qsTr("Save")
                type: 2
                onClicked: {
                    errorLabel.visible = false

                    if (ratingValue < 1) {
                        errorLabel.text = qsTr("Please select a rating")
                        errorLabel.visible = true
                        return
                    }

                    if (descriptionField.text.trim() === "") {
                        errorLabel.text = qsTr("Description cannot be empty")
                        errorLabel.visible = true
                        return
                    }

                    if (FeedbackModel.saveFeedback(ratingValue, descriptionField.text.trim())) {
                        dialog.close()
                    } else {
                        errorLabel.text = FeedbackModel.lastError
                        errorLabel.visible = true
                    }
                }
            }
        }
    }

    property int ratingValue: 0

    onOpened: {
        console.log("FeedbackDialog opened")
        errorLabel.visible = false
        
        // Load existing feedback if available
        console.log("Checking if user has feedback...")
        if (FeedbackModel.hasUserFeedback()) {
            console.log("User has feedback, loading...")
            var feedback = FeedbackModel.getUserFeedback()
            console.log("Feedback loaded:", JSON.stringify(feedback))
            if (feedback.rate !== undefined) {
                ratingValue = feedback.rate
                descriptionField.text = feedback.description
                console.log("Feedback applied: rate=" + ratingValue + ", description=" + descriptionField.text)
            } else {
                console.log("Feedback has no rate property")
            }
        } else {
            console.log("User has no feedback, resetting fields")
            ratingValue = 0
            descriptionField.text = ""
        }
    }

    function showError(msg) {
        errorLabel.text = msg
        errorLabel.visible = msg && msg.length > 0
    }
}
