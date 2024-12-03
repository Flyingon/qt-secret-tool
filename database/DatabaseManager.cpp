#include "DatabaseManager.h"


// 数据库文件名
const QString DATABASE_NAME = "passwords.db";

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{
    // 添加SQLite数据库驱动
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(DATABASE_NAME);
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

// 获取单例实例
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager singleton;
    return singleton;
}

// 打开数据库连接
bool DatabaseManager::openDatabase()
{
    if (!m_database.open()) {
        qDebug() << "无法打开数据库：" << m_database.lastError().text();
        return false;
    }

    // 创建密码表（如果不存在）
    QSqlQuery query(m_database);
    if (!query.exec("CREATE TABLE IF NOT EXISTS passwords ("
                    "account TEXT PRIMARY KEY,"
                    "password TEXT)"
                    )) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }

    return true;
}

// 关闭数据库连接
void DatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

// 添加密码记录
bool DatabaseManager::addPassword(const QString& account, const QString& password, QString &errMsg)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO passwords (account, password) VALUES (:account, :password)");
    query.bindValue(":account", account);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "添加密码记录失败：" << query.lastError().text();
        errMsg = query.lastError().text();
        return false;
    }

    return true;
}

// 根据账号删除密码记录
bool DatabaseManager::deletePassword(const QString& account)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM passwords WHERE account = :account");
    query.bindValue(":account", account);

    if (!query.exec()) {
        qDebug() << "删除密码记录失败：" << query.lastError().text();
        return false;
    }

    return true;
}

// 更新密码记录
bool DatabaseManager::updatePassword(const QString& account, const QString& newPassword)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE passwords SET password = :newPassword WHERE account = :account");
    query.bindValue(":account", account);
    query.bindValue(":newPassword", newPassword);

    if (!query.exec()) {
        qDebug() << "更新密码记录失败：" << query.lastError().text();
        return false;
    }

    return true;
}

// 根据账号查询密码记录
bool DatabaseManager::queryPassword(const QString& account, QString& password)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT password FROM passwords WHERE account = :account");
    query.bindValue(":account", account);

    if (!query.exec()) {
        qDebug() << "查询密码记录失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        password = query.value(0).toString();
        return true;
    }

    return false;
}

// 全文搜索密码记录（简单示例，可根据实际需求完善）
QList<QString> DatabaseManager::fullTextSearch(const QString& keyword)
{
    QList<QString> results;
    QSqlQuery query(m_database);
    query.prepare("SELECT account FROM passwords WHERE account LIKE :keyword OR password LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%");

    if (!query.exec()) {
        qDebug() << "全文搜索失败：" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        results.append(query.value(0).toString());
    }

    return results;
}
