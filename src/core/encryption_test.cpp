#include "encryption.h"
#include <QDebug>
#include <QString>
#include <QByteArray>

int main() {
    // 测试数据
    QByteArray originalData = "这是一个测试数据，用于验证加密和解密功能是否正常工作。";
    QString masterPassword = "test_password_123";
    
    qDebug() << "原始数据: " << originalData;
    
    // 加密数据
    QByteArray encryptedData = Encryption::encrypt(originalData, masterPassword);
    qDebug() << "加密后数据大小: " << encryptedData.size();
    
    // 解密数据
    QByteArray decryptedData = Encryption::decrypt(encryptedData, masterPassword);
    qDebug() << "解密后数据: " << decryptedData;
    
    // 验证解密是否成功
    bool success = (originalData == decryptedData);
    qDebug() << "解密是否成功: " << (success ? "是" : "否");
    
    return 0;
}