#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

/**
 * @brief 加密工具类
 * 
 * 提供加密和解密功能，用于保护密码数据
 */
class Encryption
{
public:
    // 加密常量
    static const int ITERATION_COUNT = 10000;  // PBKDF2 迭代次数
    static const int KEY_SIZE = 32;            // 256位密钥
    static const int IV_SIZE = 16;             // 初始化向量大小
    
    /**
     * @brief 使用主密码加密数据
     * @param data 要加密的数据
     * @param masterPassword 主密码
     * @return 加密后的数据
     */
    static QByteArray encrypt(const QByteArray &data, const QString &masterPassword);
    
    /**
     * @brief 使用主密码解密数据
     * @param encryptedData 加密的数据
     * @param masterPassword 主密码
     * @return 解密后的数据
     */
    static QByteArray decrypt(const QByteArray &encryptedData, const QString &masterPassword);
    
    /**
     * @brief 生成密码哈希
     * @param password 密码
     * @param salt 盐值
     * @return 哈希后的密码
     */
    static QByteArray hashPassword(const QString &password, const QByteArray &salt);
    
    /**
     * @brief 生成随机盐值
     * @param length 盐值长度
     * @return 随机盐值
     */
    static QByteArray generateSalt(int length = KEY_SIZE);
    
    /**
     * @brief 生成随机密码
     * @param length 密码长度
     * @param includeUppercase 是否包含大写字母
     * @param includeLowercase 是否包含小写字母
     * @param includeNumbers 是否包含数字
     * @param includeSpecial 是否包含特殊字符
     * @return 生成的随机密码
     */
    static QString generateRandomPassword(int length = 16, 
                                         bool includeUppercase = true,
                                         bool includeLowercase = true,
                                         bool includeNumbers = true,
                                         bool includeSpecial = true);

private:
    // 私有构造函数，防止实例化
    Encryption() {}
};

#endif // ENCRYPTION_H