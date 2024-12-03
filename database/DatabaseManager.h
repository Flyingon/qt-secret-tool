#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();

    // 打开数据库连接
    bool openDatabase();

    // 关闭数据库连接
    void closeDatabase();

    // 添加密码记录
    bool addPassword(const QString& account, const QString& password, QString &errMsg);

    // 根据账号删除密码记录
    bool deletePassword(const QString& account);

    // 更新密码记录
    bool updatePassword(const QString& account, const QString& newPassword);

    // 根据账号查询密码记录
    bool queryPassword(const QString& account, QString& password);

    // 全文搜索密码记录（简单示例，可根据实际需求完善）
    QList<QString> fullTextSearch(const QString& keyword);

private:
    DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    // 数据库对象
    QSqlDatabase m_database;

    // 私有化拷贝构造函数和赋值运算符，确保单例模式
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
