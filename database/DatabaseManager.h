#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

struct DBSecretItem {
    QString key;
    QString secret;
    QDate createdAt;
    QDate updatedAt;
};

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
    bool saveSecret(const DBSecretItem& item, QString &errMsg);

    // 根据账号删除密码记录
    bool deleteSecret(const QString& key);

    // 更新密码记录
    bool updateSecret(const DBSecretItem& item, QString &errMsg);

    // 根据账号查询密码记录
    bool querySecret(const QString& key, QString& secret);

    // 查询密码记录列表
    bool querySecretList(const QString& key, int pageIndex, int pageSize, std::vector<DBSecretItem>& resultList);

    // 全文搜索密码记录（简单示例，可根据实际需求完善）
    bool fullTextSearch(const QString& keyword, std::vector<DBSecretItem>& resultList);

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
