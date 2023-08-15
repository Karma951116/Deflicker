import QtQuick 2.14
import QtQuick.Controls 2.0

Item {
    signal minimized();
    signal maximized();
    signal restored();
    signal closed();
    signal moved(var deltaX, var deltaY)

    Text {
        id: title
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: 100
        height: parent.height
        text: qsTr("DeflickerAlgorithm")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        property point clickPos: "0,0"
        onPressed: {
            clickPos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: {
            moved(mouse.x - clickPos.x, mouse.y - clickPos.y)
        }
    }

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        Button {
            id: minimize_btn
            width: 16
            height: 16
            onClicked: {
                minimized();
            }
        }
        Button {
            id: maximize_btn
            width: 16
            height: 16
            onClicked: {
                maximized();
            }
        }
        Button {
            id: close_btn
            width: 16
            height: 16
            onClicked: {
                closed();
            }
        }
    }
}
