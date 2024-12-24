#include "DatabaseManager.h"
#include <QDir>
#include <QMessageBox>


DatabaseManager::DatabaseManager(QObject *parent)
        : QObject(parent) {
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

// 获取单例实例
DatabaseManager &DatabaseManager::instance() {
    static DatabaseManager singleton;
    return singleton;
}

// 打开数据库连接
bool DatabaseManager::openDatabase(QString dbPath, QString encryptionKey) {
    // 打开数据库
    int rc = sqlite3_open_v2(dbPath.toUtf8().constData(), &m_dbHandle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "无法打开数据库：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    // 设置加密密钥
    rc = sqlite3_key(m_dbHandle, encryptionKey.toUtf8().constData(), encryptionKey.length());
    if (rc != SQLITE_OK) {
        qDebug() << "设置加密密钥失败：" << sqlite3_errmsg(m_dbHandle);
        sqlite3_close(m_dbHandle);
        m_dbHandle = nullptr;
        return false;
    }

    // 创建表
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS t_secret ("
                                 "key TEXT PRIMARY KEY,"
                                 "secret TEXT,"
                                 "created_at DATE NOT NULL,"
                                 "updated_at DATE NOT NULL);";
    char *errMsg = nullptr;
    rc = sqlite3_exec(m_dbHandle, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        qDebug() << "创建表失败：" << errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(m_dbHandle);
        m_dbHandle = nullptr;
        return false;
    }

    return true;
}

// 关闭数据库连接
void DatabaseManager::closeDatabase() {
    if (m_dbHandle) {
        sqlite3_close(m_dbHandle);
        m_dbHandle = nullptr;
    }
}

bool DatabaseManager::saveSecret(const DBSecretItem &item, QString &errMsg) {
    // 先查询是否已存在对应key的记录
    sqlite3_stmt *stmt;
    const char *queryExistsSQL = "SELECT COUNT(*) FROM t_secret WHERE key = ?";
    int rc = sqlite3_prepare_v2(m_dbHandle, queryExistsSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        errMsg = "查询记录是否存在失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        return false;
    }

    sqlite3_bind_text(stmt, 1, item.key.toUtf8().constData(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        errMsg = "查询记录是否存在失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        sqlite3_finalize(stmt);
        return false;
    }

    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count > 0) {
        // 记录已存在，执行更新操作
        const char *updateSQL = "UPDATE t_secret SET secret = ?, updated_at = CURRENT_TIMESTAMP WHERE key = ?";
        rc = sqlite3_prepare_v2(m_dbHandle, updateSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            errMsg = "更新记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
            return false;
        }

        sqlite3_bind_text(stmt, 1, item.secret.toUtf8().constData(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.key.toUtf8().constData(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            errMsg = "更新记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
            return false;
        }
        return true;
    }

    // 如果不存在对应key的记录，则执行插入操作
    const char *insertSQL = "INSERT INTO t_secret (key, secret, created_at, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)";
    rc = sqlite3_prepare_v2(m_dbHandle, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        errMsg = "插入记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        return false;
    }

    sqlite3_bind_text(stmt, 1, item.key.toUtf8().constData(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.secret.toUtf8().constData(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        errMsg = "插入记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        return false;
    }

    return true;
}

// 根据账号删除密码记录
bool DatabaseManager::deleteSecret(const QString &key) {
    sqlite3_stmt *stmt;
    const char *deleteSQL = "DELETE FROM t_secret WHERE key = ?";
    int rc = sqlite3_prepare_v2(m_dbHandle, deleteSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "删除密码记录失败：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.toUtf8().constData(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        qDebug() << "删除密码记录失败：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    return true;
}

// 更新密码记录，使用DBSecretItem作为参数，更新secret和updated_at字段（不更新created_at字段）
bool DatabaseManager::updateSecret(const DBSecretItem &item, QString &errMsg) {
    sqlite3_stmt *stmt;
    const char *updateSQL = "UPDATE t_secret SET secret = ?, updated_at = CURRENT_TIMESTAMP WHERE key = ?";
    int rc = sqlite3_prepare_v2(m_dbHandle, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        errMsg = "更新记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        return false;
    }

    sqlite3_bind_text(stmt, 1, item.secret.toUtf8().constData(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.key.toUtf8().constData(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        errMsg = "更新记录失败：" + QString::fromUtf8(sqlite3_errmsg(m_dbHandle));
        return false;
    }

    return true;
}


// 根据账号查询密码记录
bool DatabaseManager::querySecret(const QString &key, QString &secret) {
    sqlite3_stmt *stmt;
    const char *querySQL = "SELECT secret FROM t_secret WHERE key = ?";
    int rc = sqlite3_prepare_v2(m_dbHandle, querySQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "查询密码记录失败：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.toUtf8().constData(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        secret = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
        sqlite3_finalize(stmt);
        return true;
    } else {
        sqlite3_finalize(stmt);
        return false;
    }
}


// 查询密码记录列表，支持分页查询，填充DBSecretItem包含创建时间和更新时间信息
bool DatabaseManager::querySecretList(const QString &key, int pageIndex, int pageSize,
                                      std::vector<DBSecretItem> &resultList) {
    sqlite3_stmt *stmt;
    QString queryStr = "SELECT key, secret, created_at, updated_at FROM t_secret";
    if (!key.isEmpty()) {
        queryStr += " WHERE key LIKE ?";
    }
    queryStr += " LIMIT ? OFFSET ?";

    int rc = sqlite3_prepare_v2(m_dbHandle, queryStr.toUtf8().constData(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "查询密码记录列表失败：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    if (!key.isEmpty()) {
        sqlite3_bind_text(stmt, 1, "%" + key.toUtf8() + "%", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, pageSize);
        sqlite3_bind_int(stmt, 3, pageIndex * pageSize);
    } else {
        sqlite3_bind_int(stmt, 1, pageSize);
        sqlite3_bind_int(stmt, 2, pageIndex * pageSize);
    }

    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        DBSecretItem item;
        item.key = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
        item.secret = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
        item.createdAt = QDate::fromString(
                QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))), "yyyy-MM-dd");
        item.updatedAt = QDate::fromString(
                QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))), "yyyy-MM-dd");
        resultList.push_back(item);
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    return true;
}


// 全文搜索密码记录（简单示例，可根据实际需求完善）
bool DatabaseManager::fullTextSearch(const QString &keyword, std::vector<DBSecretItem> &resultList) {
    sqlite3_stmt *stmt;
    const char *searchSQL = "SELECT key, secret, created_at, updated_at FROM t_secret WHERE key LIKE ? OR secret LIKE ?";
    int rc = sqlite3_prepare_v2(m_dbHandle, searchSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "全文搜索失败：" << sqlite3_errmsg(m_dbHandle);
        return false;
    }

    QString keywordPattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, keywordPattern.toUtf8().constData(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, keywordPattern.toUtf8().constData(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        DBSecretItem item;
        item.key = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
        item.secret = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
        item.createdAt = QDate::fromString(
                QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))), Qt::ISODate);
        item.updatedAt = QDate::fromString(
                QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))), Qt::ISODate);
        resultList.push_back(item);
    }

    if (rc != SQLITE_DONE) {
        qDebug() << "全文搜索失败：" << sqlite3_errmsg(m_dbHandle);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

