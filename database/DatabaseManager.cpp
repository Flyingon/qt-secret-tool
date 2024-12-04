#include "DatabaseManager.h"


// 数据库文件名
const QString DATABASE_NAME = "secret_tool.db";

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
    if (!query.exec("CREATE TABLE IF NOT EXISTS t_secret ("
                    "key TEXT PRIMARY KEY,"
                    "secret TEXT,"
                    "created_at DATE NOT NULL,"
                    "updated_at DATE NOT NULL)"
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

bool DatabaseManager::saveSecret(const DBSecretItem& item, QString &errMsg)
{
    QSqlQuery query(m_database);
    // 先查询是否已存在对应key的记录
    query.prepare("SELECT COUNT(*) FROM t_secret WHERE key = :key");
    query.bindValue(":key", item.key);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        if (count > 0) {
            // 记录已存在，执行更新操作，更新secret和updated_at字段
            query.prepare("UPDATE t_secret SET secret = :secret, updated_at = CURRENT_TIMESTAMP WHERE key = :key");
            query.bindValue(":secret", item.secret);
            query.bindValue(":key", item.key);
            if (!query.exec()) {
                qDebug() << "更新记录失败：" << query.lastError().text();
                errMsg = query.lastError().text();
                return false;
            }
            return true;
        }
    } else {
        qDebug() << "查询记录是否存在时失败：" << query.lastError().text();
        errMsg = query.lastError().text();
        return false;
    }

    // 如果不存在对应key的记录，则执行插入操作
    query.prepare("INSERT INTO t_secret (key, secret, created_at, updated_at) VALUES (:key, :secret, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)");
    query.bindValue(":key", item.key);
    query.bindValue(":secret", item.secret);

    if (!query.exec()) {
        qDebug() << "添加记录失败：" << query.lastError().text();
        errMsg = query.lastError().text();
        return false;
    }
    return true;
}

// 根据账号删除密码记录
bool DatabaseManager::deleteSecret(const QString& key)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM t_secret WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "删除密码记录失败：" << query.lastError().text();
        return false;
    }

    return true;
}

// 更新密码记录，使用DBSecretItem作为参数，更新secret和updated_at字段（不更新created_at字段）
bool DatabaseManager::updateSecret(const DBSecretItem& item, QString &errMsg)
{
    QSqlQuery query(m_database);
    // 构建更新语句，更新secret和updated_at字段，其中updated_at设置为CURRENT_TIMESTAMP来获取当前时间
    query.prepare("UPDATE t_secret SET secret = :secret, updated_at = CURRENT_TIMESTAMP WHERE key = :key");
    query.bindValue(":key", item.key);
    query.bindValue(":secret", item.secret);

    if (!query.exec()) {
        qDebug() << "更新密码记录失败：" << query.lastError().text();
        errMsg = query.lastError().text();
        return false;
    }

    return true;
}

// 根据账号查询密码记录
bool DatabaseManager::querySecret(const QString& key, QString& secret)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT secret FROM t_secret WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "查询密码记录失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        secret = query.value(0).toString();
        return true;
    }

    return false;
}

// 查询密码记录列表，支持分页查询，填充DBSecretItem包含创建时间和更新时间信息
bool DatabaseManager::querySecretList(const QString& key, int pageIndex, int pageSize, std::vector<DBSecretItem>& resultList)
{
    QSqlQuery query(m_database);
    // 构建带有分页逻辑的查询语句，这里假设每页显示pageSize条记录，查询第pageIndex页的数据
    QString queryStr = QString("SELECT key, secret, created_at, updated_at FROM t_secret");
    if (!key.isEmpty()) {
        queryStr += QString(" WHERE key LIKE :key");
        query.prepare(queryStr);
        query.bindValue(":key", "%" + key + "%");
    } else {
        query.prepare(queryStr);
    }
    queryStr += QString(" LIMIT %1 OFFSET %2").arg(pageSize).arg(pageIndex * pageSize);
    if (!query.exec(queryStr)) {
        qDebug() << "查询密码记录列表失败：" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        DBSecretItem item;
        item.key = query.value(0).toString();
        item.secret = query.value(1).toString();
        item.createdAt = query.value(2).toDate();  // 假设数据库中created_at字段对应的是日期类型，可根据实际情况调整类型转换
        item.updatedAt = query.value(3).toDate();  // 同理，假设updated_at字段对应的是日期类型
        resultList.push_back(item);
    }
    return true;
}

// 全文搜索密码记录（简单示例，可根据实际需求完善）
QList<QString> DatabaseManager::fullTextSearch(const QString& keyword)
{
    QList<QString> results;
    QSqlQuery query(m_database);
    query.prepare("SELECT key FROM t_secret WHERE key LIKE :keyword OR password LIKE :keyword");
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
