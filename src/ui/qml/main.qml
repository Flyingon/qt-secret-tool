import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 6.2
import com.flyingontemp.secretool 1.0

Window {
    id: mainWindow
    width: 900
    height: 600
    visible: true
    title: qsTr("密码管理器")
    
    // 主密码对话框
    Dialog {
        id: masterPasswordDialog
        title: "输入主密码"
        modal: true
        width: 400
        height: 200
        
        footer: DialogButtonBox {
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
            onAccepted: {
                if (masterPasswordInput.text.length > 0) {
                    if (passwordManager.initialize(masterPasswordInput.text)) {
                        masterPasswordInput.text = ""
                        masterPasswordDialog.close()
                    } else {
                        errorDialog.text = "初始化失败，请检查主密码"
                        errorDialog.open()
                    }
                } else {
                    errorDialog.text = "主密码不能为空"
                    errorDialog.open()
                }
            }
            onRejected: masterPasswordDialog.reject()
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: "请输入主密码以解锁数据库"
                font.pixelSize: 14
            }
            
            TextField {
                id: masterPasswordInput
                Layout.fillWidth: true
                echoMode: TextInput.Password
                placeholderText: "主密码"
                
                Keys.onReturnPressed: {
                    if (text.length > 0) {
                        masterPasswordDialog.accept()
                    }
                }
            }
        }
    }
    
    // 错误对话框
    Dialog {
        id: errorDialog
        title: "错误"
        width: 300
        height: 150
        property string text: ""
        
        footer: DialogButtonBox {
            standardButtons: DialogButtonBox.Ok
            onAccepted: errorDialog.accept()
        }
        
        contentItem: Label {
            text: errorDialog.text
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.fillHeight: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    
    // 主布局
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // 工具栏
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "添加密码"
                icon.name: "add"
                onClicked: addPasswordDialog.open()
            }
            
            Button {
                text: "导入"
                icon.name: "document-open"
                onClicked: importDialog.open()
            }
            
            Button {
                text: "导出"
                icon.name: "document-save"
                onClicked: exportDialog.open()
            }
            
            Item { Layout.fillWidth: true }
            
            SearchBar {
                id: searchBar
                Layout.preferredWidth: 300
                onSearchTextChanged: passwordManager.searchText = searchText
            }
        }
        
        // 密码列表和详情
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal
            
            PasswordList {
                id: passwordList
                SplitView.preferredWidth: 300
                SplitView.minimumWidth: 200
                model: passwordManager
                onPasswordSelected: passwordDetail.passwordId = id
            }
            
            PasswordDetail {
                id: passwordDetail
                SplitView.fillWidth: true
                SplitView.minimumWidth: 300
                onEditPassword: {
                    editPasswordDialog.passwordId = id
                    editPasswordDialog.loadPassword(id)
                    editPasswordDialog.open()
                }
                onDeletePassword: function(id) {
                    deleteConfirmDialog.passwordId = id
                    deleteConfirmDialog.open()
                }
            }
        }
        
        // 状态栏
        Rectangle {
            Layout.fillWidth: true
            height: 30
            color: "#f0f0f0"
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                
                Label {
                    text: passwordManager.isFiltered ? 
                          "显示 " + passwordManager.count + " 个结果" : 
                          "共 " + passwordManager.count + " 个密码"
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: "Qt Secret Tool v1.0"
                    font.pixelSize: 12
                    color: "#888888"
                }
            }
        }
    }
    
    // 添加密码对话框
    Dialog {
        id: addPasswordDialog
        title: "添加密码"
        standardButtons: Dialog.Save | Dialog.Cancel
        width: 500
        height: 400
        modal: true
        
        onAccepted: {
            if (addPasswordContent.titleField.text.length === 0) {
                errorDialog.text = "标题不能为空"
                errorDialog.open()
                return
            }
            
            if (addPasswordContent.passwordField.text.length === 0) {
                errorDialog.text = "密码不能为空"
                errorDialog.open()
                return
            }
            
            if (passwordManager.addPassword(
                    addPasswordContent.titleField.text,
                    addPasswordContent.usernameField.text,
                    addPasswordContent.passwordField.text,
                    addPasswordContent.urlField.text,
                    addPasswordContent.notesField.text,
                    addPasswordContent.categoryField.text
                )) {
                clearAddPasswordInputs()
                addPasswordDialog.close()
            } else {
                errorDialog.text = "添加密码失败"
                errorDialog.open()
            }
        }
        
        function clearAddPasswordInputs() {
            addPasswordContent.titleField.text = ""
            addPasswordContent.usernameField.text = ""
            addPasswordContent.passwordField.text = ""
            addPasswordContent.urlField.text = ""
            addPasswordContent.notesField.text = ""
            addPasswordContent.categoryField.text = ""
        }
        
        AddPassword {
            id: addPasswordContent
            anchors.fill: parent
            
            onGeneratePassword: {
                passwordField.text = passwordManager.generatePassword(
                    length, 
                    includeUppercase, 
                    includeLowercase, 
                    includeNumbers, 
                    includeSpecial
                )
            }
        }
    }
    
    // 编辑密码对话框
    Dialog {
        id: editPasswordDialog
        title: "编辑密码"
        standardButtons: Dialog.Save | Dialog.Cancel
        width: 500
        height: 400
        modal: true
        
        property string passwordId: ""
        
        function loadPassword(id) {
            var item = passwordManager.getPassword(id)
            if (item) {
                editPasswordContent.titleField.text = item.title
                editPasswordContent.usernameField.text = item.username
                editPasswordContent.passwordField.text = "" // 不显示密码
                editPasswordContent.urlField.text = item.url
                editPasswordContent.notesField.text = item.notes
                editPasswordContent.categoryField.text = item.category
            }
        }
        
        onAccepted: {
            if (passwordManager.updatePassword(
                    passwordId,
                    editPasswordContent.titleField.text,
                    editPasswordContent.usernameField.text,
                    editPasswordContent.passwordField.text,
                    editPasswordContent.urlField.text,
                    editPasswordContent.notesField.text,
                    editPasswordContent.categoryField.text
                )) {
                passwordDetail.refresh()
            } else {
                errorDialog.text = "更新密码失败"
                errorDialog.open()
            }
        }
        
        AddPassword {
            id: editPasswordContent
            anchors.fill: parent
            
            onGeneratePassword: {
                passwordField.text = passwordManager.generatePassword(
                    length, 
                    includeUppercase, 
                    includeLowercase, 
                    includeNumbers, 
                    includeSpecial
                )
            }
        }
    }
    
    // 删除确认对话框
    Dialog {
        id: deleteConfirmDialog
        title: "确认删除"
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        
        property string passwordId: ""
        
        Label {
            text: "确定要删除这个密码吗？此操作不可撤销。"
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (passwordManager.deletePassword(passwordId)) {
                passwordDetail.passwordId = ""
            } else {
                errorDialog.text = "删除密码失败"
                errorDialog.open()
            }
        }
    }
    
    // 导入对话框
    Dialog {
        id: importDialog
        title: "导入密码"
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 400
        height: 200
        modal: true
        
        property string importFilePath: ""
        property string importFormat: "json"
        
        onAccepted: {
            if (importFilePath) {
                if (passwordManager.importPasswords(importFilePath, importFormat)) {
                    importFilePath = ""
                } else {
                    errorDialog.text = "导入密码失败"
                    errorDialog.open()
                }
            } else {
                errorDialog.text = "请选择导入文件"
                errorDialog.open()
            }
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: "选择要导入的文件:"
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                TextField {
                    Layout.fillWidth: true
                    readOnly: true
                    text: importDialog.importFilePath
                    placeholderText: "未选择文件"
                }
                
                Button {
                    text: "浏览..."
                    onClicked: {
                        var path = fileManager.getOpenFilePath(
                            "选择导入文件",
                            "",
                            "JSON 文件 (*.json);;CSV 文件 (*.csv);;所有文件 (*.*)"
                        )
                        if (path) {
                            importDialog.importFilePath = path
                            importDialog.importFormat = fileManager.getFileExtension(path)
                        }
                    }
                }
            }
            
            Label {
                text: "导入格式:"
            }
            
            ComboBox {
                Layout.fillWidth: true
                model: ["json", "csv"]
                currentIndex: importDialog.importFormat === "json" ? 0 : 1
                onCurrentTextChanged: importDialog.importFormat = currentText
            }
        }
    }
    
    // 导出对话框
    Dialog {
        id: exportDialog
        title: "导出密码"
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 400
        height: 200
        modal: true
        
        property string exportFilePath: ""
        property string exportFormat: "json"
        
        onAccepted: {
            if (exportFilePath) {
                if (passwordManager.exportPasswords(exportFilePath, exportFormat)) {
                    exportFilePath = ""
                } else {
                    errorDialog.text = "导出密码失败"
                    errorDialog.open()
                }
            } else {
                errorDialog.text = "请选择导出文件"
                errorDialog.open()
            }
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: "选择导出格式:"
            }
            
            ComboBox {
                Layout.fillWidth: true
                model: ["json", "csv"]
                onCurrentTextChanged: {
                    exportDialog.exportFormat = currentText
                }
            }
            
            Label {
                text: "选择保存位置:"
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                TextField {
                    Layout.fillWidth: true
                    readOnly: true
                    text: exportDialog.exportFilePath
                    placeholderText: "未选择文件"
                }
                
                Button {
                    text: "浏览..."
                    onClicked: {
                        var filter = exportDialog.exportFormat === "json" ? 
                                    "JSON 文件 (*.json)" : "CSV 文件 (*.csv)"
                        var path = fileManager.getSaveFilePath(
                            "保存导出文件",
                            "",
                            filter
                        )
                        if (path) {
                            exportDialog.exportFilePath = path
                        }
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        masterPasswordDialog.open()
    }
}