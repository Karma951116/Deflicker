import QtQuick 2.14
import QtQuick.Window 2.3

import "qrc:/Components/"

Window {
    id: root
    property bool bMaximized: false
    signal showHist()

    width: 2560
    height: 1440
//    width: Screen.width
//    height: Screen.height
    screen: Qt.application.screens[0]
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"

    MediaPanel {
        id: media_panel
        anchors.fill: parent
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: StandardKey.MoveToPreviousChar
        onActivated: {
            srcBuffer.prev();
            dstBuffer.prev();
            showHist();
        }
    }
    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: StandardKey.MoveToNextChar
        onActivated: {
            srcBuffer.next();
            dstBuffer.next();
            showHist();
        }
    }


    Connections {
        target: media_panel.header
        function onMoved(deltaX, deltaY) {
            root.setX(root.x + deltaX);
            root.setY(root.y + deltaY);
        }
        function onMinimized() {
            root.showMinimized();
        }
        function onMaximized() {
            if (bMaximized) {
                root.showNormal();
            }
            else {
                root.showMaximized();
            }
            bMaximized = !bMaximized;
        }
        function onClosed() {
            root.close();
        }
    }
}
