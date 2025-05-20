import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: 36
    radius: 18
    color: "#f0f0f0"
    border.color: searchField.activeFocus ? "#4285f4" : "#dddddd"
    
    property string searchText: ""
    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 5
        
        Image {
            source: "qrc:/icons/icons/search.png"
            width: 16
            height: 16
            sourceSize.width: 16
            sourceSize.height: 16
            
            // 如果没有图标，可以使用 Text 代替
            visible: false
        }
        
        // 搜索图标替代方案
        Text {
            text: "🔍"
            font.pixelSize: 16
            color: "#888888"
        }
        
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "搜索密码..."
            background: null
            selectByMouse: true
            
            onTextChanged: {
                root.searchText = text
            }
        }
        
        // 清除按钮
        Text {
            text: "✕"
            font.pixelSize: 14
            color: "#888888"
            visible: searchField.text.length > 0
            
            MouseArea {
                anchors.fill: parent
                anchors.margins: -5
                onClicked: {
                    searchField.text = ""
                    searchField.forceActiveFocus()
                }
            }
        }
    }
}