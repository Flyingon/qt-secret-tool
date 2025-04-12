#include "encryption.h"
#include <QCryptographicHash>
#include <QSettings>
#include <QRandomGenerator>
#include <QDebug>

Encryption::Encryption()
{
    // 尝试从设置加载密钥
    if (!loadKeyFromSettings()) {
        // 如果没有保存的密钥，创建一个空密钥
        m_key = QByteArray(32, 0); // AES-256 需要32字节密钥
        m_iv = QByteArray(16, 0);  // AES 需要16字节IV
    }
}

bool Encryption::setMasterPassword(const QString &password)
{
    deriveKeyFromPassword(password);
    return saveKeyToSettings();
}

bool Encryption::checkMasterPassword(const QString &password)
{
    QByteArray currentKey = m_key;
    QByteArray currentIv = m_iv;
    
    // 从输入的密码派生密钥
    deriveKeyFromPassword(password);
    
    // 检查派生的密钥是否与当前密钥匹配
    bool result = (m_key == currentKey && m_iv == currentIv);
    
    // 如果不匹配，恢复原始密钥
    if (!result) {
        m_key = currentKey;
        m_iv = currentIv;
    }
    
    return result;
}

QString Encryption::encrypt(const QString &plaintext)
{
    // 这里使用简单的异或加密作为示例
    // 在实际应用中，应该使用更强大的加密库，如OpenSSL
    QByteArray data = plaintext.toUtf8();
    QByteArray encrypted;
    
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data.at(i) ^ m_key.at(i % m_key.size()));
    }
    
    return QString::fromLatin1(encrypted.toBase64());
}

QString Encryption::decrypt(const QString &ciphertext)
{
    // 解密过程
    QByteArray data = QByteArray::fromBase64(ciphertext.toLatin1());
    QByteArray decrypted;
    
    for (int i = 0; i < data.size(); ++i) {
        decrypted.append(data.at(i) ^ m_key.at(i % m_key.size()));
    }
    
    return QString::fromUtf8(decrypted);
}

QString Encryption::generateRandomPassword(int length, bool includeUppercase, 
                                          bool includeLowercase, bool includeNumbers, 
                                          bool includeSpecial)
{
    QString chars;
    if (includeUppercase) chars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (includeLowercase) chars += "abcdefghijklmnopqrstuvwxyz";
    if (includeNumbers) chars += "0123456789";
    if (includeSpecial) chars += "!@#$%^&*()-_=+[]{}|;:,.<>?";
    
    if (chars.isEmpty()) {
        // 默认至少包含小写字母
        chars = "abcdefghijklmnopqrstuvwxyz";
    }
    
    QString result;
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        result.append(chars.at(index));
    }
    
    return result;
}

void Encryption::deriveKeyFromPassword(const QString &password)
{
    // 使用PBKDF2算法从密码派生密钥
    // 这里使用简化版本，实际应用中应使用更安全的实现
    QByteArray salt = "QtSecretToolSalt"; // 在实际应用中应使用随机盐
    QByteArray passwordData = password.toUtf8();
    
    // 使用SHA-256哈希函数进行多次迭代
    QByteArray hash = passwordData + salt;
    for (int i = 0; i < 10000; ++i) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }
    
    // 设置密钥和IV
    m_key = hash.left(32); // 取前32字节作为密钥
    m_iv = hash.right(16); // 取后16字节作为IV
}

bool Encryption::saveKeyToSettings()
{
    // 注意：在实际应用中，不应该直接保存密钥
    // 这里仅作为示例，应该使用更安全的方法存储密钥
    QSettings settings;
    settings.setValue("encryption/keyCheck", QCryptographicHash::hash(m_key, QCryptographicHash::Sha256).toHex());
    return true;
}

bool Encryption::loadKeyFromSettings()
{
    // 检查是否有保存的密钥校验值
    QSettings settings;
    return settings.contains("encryption/keyCheck");
}