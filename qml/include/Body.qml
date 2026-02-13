import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import pages
import include

SwipeView {
    id: swipeView
    anchors.fill: parent
    interactive: false

    TasksPage {
        width: swipeView.width
        height: swipeView.height
    }

    ChartsPage {
        width: swipeView.width
        height: swipeView.height
    }

    Connections {
        target: LanguageManager
        function onLanguageChanged() {
            // After language change and retranslate, restore ComboBox indices
            if (parent && parent.header && parent.header.restoreComboBoxIndices) {
                parent.header.restoreComboBoxIndices()
            }
        }
    }
    Connections {
        target: footer
        function onCurrentIndexChanged() {
            swipeView.currentIndex = footer.currentIndex
        }
    }
}
