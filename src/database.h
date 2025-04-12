#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "passworditem.h"
#include "encryption.h"

class Database : public QObject
{
    Q_OBJECT
    
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    
    // 初始化数据库
    bool initialize();
    
    // 设置加密实例
    void setEncryption(Encryption *encryption);
    
    // 密码项操作
    bool addPassword(PasswordItem &item);
    bool updatePassword(const PasswordItem &item);
    bool deletePassword(int id);
    PasswordItem getPassword(int id);
    QList<PasswordItem> getAllPasswords();
    QList<PasswordItem> searchPasswords(const QString &query);
    
    // 获取所有分类
    QStringList getAllCategories();
    
    // 导入导出
    bool importFromCsv(const QString &filename);
    bool exportToCsv(const QString &filename);
    bool importFromJson(const QString &filename);
    bool exportToJson(const QString &filename);
    
private:
    QSqlDatabase m_db;
    Encryption *m_encryption;
    
    // 创建表
    bool createTables();
};

#endif // DATABASE_H