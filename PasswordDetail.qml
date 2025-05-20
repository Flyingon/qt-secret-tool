onClicked: {
                            if (passwordItem) {
                                copyInProgress = true;
                                console.log("尝试解密密码，ID:", passwordItem.id);
                                // 尝试使用正确的方法名
                                passwordManager.decryptPassword(passwordItem.id)
                            } else {
                                console.log("错误：passwordItem 为 null");
                            }
                        }
    Connections {
        target: passwordManager
        
        function onPasswordDecrypted(password) {
            console.log("收到 passwordDecrypted 信号，密码长度:", password.length);
            decryptedPassword = password;
            
            // 如果是显示密码，更新显示
            if (showPasswordCheckBox.checked) {
                passwordField.text = decryptedPassword;
                console.log("已更新密码显示");
            }
            
            // 如果是复制操作，直接复制
            if (copyInProgress) {
                clipboard.setText(decryptedPassword);
                copyNotification.open();
                copyInProgress = false;
                console.log("已复制密码到剪贴板");
            }
        }
    }