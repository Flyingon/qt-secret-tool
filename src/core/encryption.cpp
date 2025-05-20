#include "encryption.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>

QByteArray Encryption::encrypt(const QByteArray &data, const QString &masterPassword)
{
    // 生成随机盐值
    QByteArray salt = generateSalt();
    
    // 从主密码生成密钥
    QByteArray key = hashPassword(masterPassword, salt);
    
    // 生成随机初始化向量
    QByteArray iv = generateSalt(IV_SIZE);
    
    // 这里使用简化的加密方法，实际应用中应使用更安全的库如 OpenSSL
    // 简单的异或加密，仅用于演示
    QByteArray encrypted;
    encrypted.reserve(data.size());
    
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data.at(i) ^ key.at(i % key.size()) ^ iv.at(i % iv.size()));
    }
    
    // 返回格式: salt + iv + encrypted data
    return salt + iv + encrypted;
}

QByteArray Encryption::decrypt(const QByteArray &encryptedData, const QString &masterPassword)
{
    // 检查数据长度是否合理
    if (encryptedData.size() <= (KEY_SIZE + IV_SIZE)) {
        return QByteArray();
    }
    
    // 提取盐值、初始化向量和加密数据
    QByteArray salt = encryptedData.left(KEY_SIZE);
    QByteArray iv = encryptedData.mid(KEY_SIZE, IV_SIZE);
    QByteArray data = encryptedData.mid(KEY_SIZE + IV_SIZE);
    
    // 从主密码和盐值生成密钥
    QByteArray key = hashPassword(masterPassword, salt);
    
    // 解密数据
    QByteArray decrypted;
    decrypted.reserve(data.size());
    
    for (int i = 0; i < data.size(); ++i) {
        decrypted.append(data.at(i) ^ key.at(i % key.size()) ^ iv.at(i % iv.size()));
    }
    
    return decrypted;
}

QByteArray Encryption::hashPassword(const QString &password, const QByteArray &salt)
{
    // 使用 PBKDF2 算法（这里用简化版模拟）
    QByteArray key = password.toUtf8() + salt;
    
    // 多次迭代哈希以增加安全性
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
    }
    
    return key;
}

QByteArray Encryption::generateSalt(int length)
{
    QByteArray salt;
    salt.reserve(length);
    
    // 使用 Qt 的随机数生成器
    QRandomGenerator *rng = QRandomGenerator::global();
    
    for (int i = 0; i < length; ++i) {
        salt.append(static_cast<char>(rng->bounded(256)));
    }
    
    return salt;
}

QString Encryption::generateRandomPassword(int length, 
                                          bool includeUppercase,
                                          bool includeLowercase,
                                          bool includeNumbers,
                                          bool includeSpecial)
{
    // 定义字符集
    const QString uppercaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const QString lowercaseChars = "abcdefghijklmnopqrstuvwxyz";
    const QString numberChars = "0123456789";
    const QString specialChars = "!@#$%^&*()-_=+[]{}|;:,.<>?";
    
    // 组合所选字符集
    QString charset;
    if (includeUppercase) charset += uppercaseChars;
    if (includeLowercase) charset += lowercaseChars;
    if (includeNumbers) charset += numberChars;
    if (includeSpecial) charset += specialChars;
    
    // 如果没有选择任何字符集，默认使用小写字母和数字
    if (charset.isEmpty()) {
        charset = lowercaseChars + numberChars;
    }
    
    // 生成随机密码
    QString password;
    QRandomGenerator *rng = QRandomGenerator::global();
    
    for (int i = 0; i < length; ++i) {
        int index = rng->bounded(charset.length());
        password.append(charset.at(index));
    }
    
    return password;
}