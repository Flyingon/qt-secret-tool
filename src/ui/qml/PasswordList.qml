import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#f5f5f5"
    border.color: "#dddddd"
    
    property var model
    signal passwordSelected(string id)
    
    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 1
        clip: true
        model: root.model
        
        delegate: ItemDelegate {
            width: listView.width
            height: 60
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4
                
                Label {
                    text: model.title
                    font.bold: true
                    font.pixelSize: 14
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                
                Label {
                    text: model.username
                    font.pixelSize: 12
                    color: "#666666"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
            
            onClicked: {
                listView.currentIndex = index
                root.passwordSelected(model.id)
            }
            
            Rectangle {
                width: parent.width
                height: 1
                color: "#dddddd"
                anchors.bottom: parent.bottom
            }
            
            // 高亮当前选中项
            Rectangle {
                anchors.fill: parent
                color: "#e0e0ff"
                opacity: listView.currentIndex === index ? 0.5 : 0
                z: -1
            }
        }
        
        ScrollBar.vertical: ScrollBar {}
    }
}