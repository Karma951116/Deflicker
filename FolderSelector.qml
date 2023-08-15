import QtQuick 2.14
import QtQuick.Controls 2.0
import Qt.labs.platform 1.1

Rectangle {
    id: folder_selector
    implicitWidth: 400
    implicitHeight: 40
    color: "gray"
    Row {
        Text {
            id: path_holder
            text: qsTr("path...")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            id: browse_btn
            width: 50
            height: 35
            text: qsTr("浏览...")
            onClicked: {
                folderDialog.open();
            }
        }
        Button {
            id: confirm_btn
            width: 50
            height: 35
            text: qsTr("确认")
            onClicked: {
                srcBuffer.clear();
                imageLoader.load(folderDialog.folder);
            }
        }
    }
    FolderDialog {
        id: folderDialog
        options: FolderDialog.ShowDirsOnly
        onAccepted: {
            path_holder.text = folderDialog.folder
        }
    }
}
