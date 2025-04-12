#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

class Encryption
{
public:
    Encryption();
    
    // 设置主密码
    bool setMasterPassword(const QString &password);
    
    // 检查主密码
    bool checkMasterPassword(const QString &password);
    
    // 加密数据
    QString encrypt(const QString &plaintext);
    
    // 解密数据
    QString decrypt(const QString &ciphertext);
    
    // 生成随机密码
    static QString generateRandomPassword(int length = 12, bool includeUppercase = true,
                                         bool includeLowercase = true, bool includeNumbers = true,
                                         bool includeSpecial = true);
    
private:
    QByteArray m_key;
    QByteArray m_iv;
    
    // 从主密码派生密钥
    void deriveKeyFromPassword(const QString &password);
    
    // 保存密钥到设置
    bool saveKeyToSettings();
    
    // 从设置加载密钥
    bool loadKeyFromSettings();
};

#endif // ENCRYPTION_H