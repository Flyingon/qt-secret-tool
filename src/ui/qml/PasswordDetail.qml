import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 6.2

Rectangle {
    id: root
    color: "#ffffff"
    
    property string passwordId: ""
    property var passwordItem: null
    property string decryptedPassword: "" // 添加一个属性存储解密后的密码
    
    signal editPassword(string id)
    signal deletePassword(string id)
    
    function refresh() {
        if (passwordId) {
            passwordItem = passwordManager.getPassword(passwordId)
        } else {
            passwordItem = null
        }
    }
    
    onPasswordIdChanged: {
        refresh()
        decryptedPassword = "" // 重置解密后的密码
    }
    
    // 只保留一个Connections对象
    Connections {
        target: passwordManager
        
        function onPasswordDecrypted(password) {
            decryptedPassword = password;
            
            // 如果是显示密码，更新显示
            if (showPasswordCheckBox.checked) {
                passwordField.text = decryptedPassword;
            }
            
            // 如果是复制操作，直接复制
            if (copyInProgress) {
                clipboard.setText(decryptedPassword);
                copyNotification.open();
                copyInProgress = false;
            }
        }
    }
    
    property bool copyInProgress: false
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // 无选中密码时显示的提示
        Label {
            text: "请从左侧列表选择一个密码项"
            font.pixelSize: 16
            Layout.alignment: Qt.AlignCenter
            visible: !passwordItem
            Layout.fillHeight: true
        }
        
        // 密码详情
        ColumnLayout {
            visible: passwordItem !== null
            spacing: 15
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            // 标题和操作按钮
            RowLayout {
                Layout.fillWidth: true
                
                Label {
                    text: passwordItem ? passwordItem.title : ""
                    font.pixelSize: 20
                    font.bold: true
                    Layout.fillWidth: true
                }
                
                Button {
                    text: "编辑"
                    icon.name: "edit"
                    onClicked: {
                        if (passwordItem) {
                            root.editPassword(passwordItem.id)
                        }
                    }
                }
                
                Button {
                    text: "删除"
                    icon.name: "delete"
                    onClicked: {
                        if (passwordItem) {
                            root.deletePassword(passwordItem.id)
                        }
                    }
                }
            }
            
            Rectangle {
                height: 1
                color: "#dddddd"
                Layout.fillWidth: true
            }
            
            // 详细信息
            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 15
                Layout.fillWidth: true
                
                Label {
                    text: "用户名:"
                    font.bold: true
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: usernameField
                        text: passwordItem ? passwordItem.username : ""
                        readOnly: true
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        text: "复制"
                        icon.name: "copy"
                        onClicked: {
                            usernameField.selectAll()
                            usernameField.copy()
                            usernameField.deselect()
                        }
                    }
                }
                
                Label {
                    text: "密码:"
                    font.bold: true
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: passwordField
                        text: "••••••••"
                        readOnly: true
                        echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        text: "复制"
                        icon.name: "copy"
                        onClicked: {
                            if (passwordItem) {
                                copyInProgress = true;
                                // 尝试使用正确的方法名
                                passwordManager.decryptPassword(passwordItem.id)
                            }
                        }
                    }
                }
                
                Item { width: 1 }
                
                CheckBox {
                    id: showPasswordCheckBox
                    text: "显示密码"
                    checked: false
                    onCheckedChanged: {
                        if (checked && passwordItem) {
                            // 如果已有解密后的密码，直接使用
                            if (decryptedPassword) {
                                passwordField.text = decryptedPassword;
                            } else {
                                // 否则请求解密
                                passwordField.text = "解密中...";
                                passwordManager.decryptPassword(passwordItem.id);
                            }
                        } else {
                            passwordField.text = "••••••••";
                        }
                    }
                }
                
                Label {
                    text: "网址:"
                    font.bold: true
                    visible: passwordItem && passwordItem.url
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    visible: passwordItem && passwordItem.url
                    
                    TextField {
                        id: urlField
                        text: passwordItem ? passwordItem.url : ""
                        readOnly: true
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        text: "复制"
                        icon.name: "copy"
                        onClicked: {
                            urlField.selectAll()
                            urlField.copy()
                            urlField.deselect()
                        }
                    }
                    
                    Button {
                        text: "访问"
                        icon.name: "web"
                        onClicked: {
                            if (passwordItem && passwordItem.url) {
                                Qt.openUrlExternally(passwordItem.url)
                            }
                        }
                    }
                }
                
                Label {
                    text: "分类:"
                    font.bold: true
                    visible: passwordItem && passwordItem.category
                }
                
                Label {
                    text: passwordItem ? passwordItem.category : ""
                    Layout.fillWidth: true
                    visible: passwordItem && passwordItem.category
                }
                
                Label {
                    text: "备注:"
                    font.bold: true
                    visible: passwordItem && passwordItem.notes
                }
                
                TextArea {
                    text: passwordItem ? passwordItem.notes : ""
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    visible: passwordItem && passwordItem.notes
                    background: Rectangle {
                        color: "#f5f5f5"
                        border.color: "#dddddd"
                    }
                }
            }
            
            Item { Layout.fillHeight: true }
            
            // 创建和修改时间
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5
                visible: passwordItem !== null
                
                Rectangle {
                    height: 1
                    color: "#dddddd"
                    Layout.fillWidth: true
                }
                
                Label {
                    text: "创建时间: " + (passwordItem ? Qt.formatDateTime(passwordItem.createdAt, "yyyy-MM-dd hh:mm:ss") : "")
                    font.pixelSize: 12
                    color: "#888888"
                }
                
                Label {
                    text: "修改时间: " + (passwordItem ? Qt.formatDateTime(passwordItem.modifiedAt, "yyyy-MM-dd hh:mm:ss") : "")
                    font.pixelSize: 12
                    color: "#888888"
                }
            }
        }
    }
    
    // 复制通知
    Popup {
        id: copyNotification
        x: (parent.width - width) / 2
        y: parent.height - height - 20
        width: 200
        height: 40
        modal: false
        focus: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        background: Rectangle {
            color: "#333333"
            radius: 5
        }
        
        Label {
            anchors.centerIn: parent
            text: "已复制到剪贴板"
            color: "white"
        }
        
        Timer {
            running: copyNotification.visible
            interval: 2000
            onTriggered: copyNotification.close()
        }
    }
    
    // 恢复原来的剪贴板对象
    Item {
        id: clipboard
        
        function setText(text) {
            clipboardText.text = text
            clipboardText.selectAll()
            clipboardText.copy()
            clipboardText.deselect()
        }
        
        TextEdit {
            id: clipboardText
            visible: false
        }
    }
}