import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 6.2

Item {
    id: root
    
    property alias titleField: titleInput
    property alias usernameField: usernameInput
    property alias passwordField: passwordInput
    property alias urlField: urlInput
    property alias notesField: notesInput
    property alias categoryField: categoryInput
    
    signal generatePassword(int length, bool includeUppercase, bool includeLowercase, bool includeNumbers, bool includeSpecial)
    
    ScrollView {
        anchors.fill: parent
        clip: true
        
        ColumnLayout {
            width: root.width - 20
            spacing: 15
            
            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 15
                Layout.fillWidth: true
                
                Label {
                    text: "标题:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
                
                TextField {
                    id: titleInput
                    placeholderText: "网站或应用名称"
                    Layout.fillWidth: true
                }
                
                Label {
                    text: "用户名:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
                
                TextField {
                    id: usernameInput
                    placeholderText: "电子邮件或用户名"
                    Layout.fillWidth: true
                }
                
                Label {
                    text: "密码:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: passwordInput
                        placeholderText: "密码"
                        echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        text: "生成"
                        onClicked: passwordGeneratorDialog.open()
                    }
                }
                
                Item { width: 1 }
                
                CheckBox {
                    id: showPasswordCheckBox
                    text: "显示密码"
                    checked: false
                }
                
                Label {
                    text: "网址:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
                
                TextField {
                    id: urlInput
                    placeholderText: "https://"
                    Layout.fillWidth: true
                }
                
                Label {
                    text: "分类:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
                
                TextField {
                    id: categoryInput
                    placeholderText: "可选分类"
                    Layout.fillWidth: true
                }
                
                Label {
                    text: "备注:"
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                }
                
                TextArea {
                    id: notesInput
                    placeholderText: "添加备注信息"
                    wrapMode: TextEdit.Wrap
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                }
            }
        }
    }
    
    // 密码生成器对话框
    Dialog {
        id: passwordGeneratorDialog
        title: "密码生成器"
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 400
        height: 300
        
        onAccepted: {
            root.generatePassword(
                lengthSlider.value,
                uppercaseCheckBox.checked,
                lowercaseCheckBox.checked,
                numbersCheckBox.checked,
                specialCheckBox.checked
            )
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 15
            
            Label {
                text: "密码长度: " + lengthSlider.value
            }
            
            Slider {
                id: lengthSlider
                from: 8
                to: 32
                stepSize: 1
                value: 16
                Layout.fillWidth: true
            }
            
            CheckBox {
                id: uppercaseCheckBox
                text: "包含大写字母 (A-Z)"
                checked: true
            }
            
            CheckBox {
                id: lowercaseCheckBox
                text: "包含小写字母 (a-z)"
                checked: true
            }
            
            CheckBox {
                id: numbersCheckBox
                text: "包含数字 (0-9)"
                checked: true
            }
            
            CheckBox {
                id: specialCheckBox
                text: "包含特殊字符 (!@#$%^&*)"
                checked: true
            }
        }
    }
}