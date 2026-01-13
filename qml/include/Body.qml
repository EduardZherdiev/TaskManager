import QtQuick 2.11
import QtQuick.Controls 2.11
import StyleModule 1.0
import pages

SwipeView {
    id: swipeView
    anchors.fill: parent
    interactive: false
    currentIndex: footer.currentIndex

    TasksPage {
        width: swipeView.width
        height: swipeView.height
    }

    ChartsPage {
        width: swipeView.width
        height: swipeView.height
    }
}
