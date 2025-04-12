#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>

Database::Database(QObject *parent)
    : QObject(parent)
    , m_encryption(nullptr)
{
}

Database::~Database()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::initialize()
{
    // 设置数据库文件路径
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString dbPath = dir.filePath("passwords.db");
    
    // 连接数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    
    if (!m_db.open()) {
        qDebug() << "无法打开数据库:" << m_db.lastError().text();
        return false;
    }
    
    // 创建表
    return createTables();
}

void Database::setEncryption(Encryption *encryption)
{
    m_encryption = encryption;
}

bool Database::createTables()
{
    QSqlQuery query;
    
    // 创建密码表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS passwords ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "username TEXT, "
        "password TEXT NOT NULL, "
        "url TEXT, "
        "notes TEXT, "
        "category TEXT, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")"
    );
    
    if (!success) {
        qDebug() << "创建密码表失败:" << query.lastError().text();
        return false;
    }
    
    // 创建设置表
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS settings ("
        "key TEXT PRIMARY KEY, "
        "value TEXT"
        ")"
    );
    
    if (!success) {
        qDebug() << "创建设置表失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::addPassword(PasswordItem &item)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return false;
    }
    
    QSqlQuery query;
    query.prepare(
        "INSERT INTO passwords (title, username, password, url, notes, category, created_at, updated_at) "
        "VALUES (:title, :username, :password, :url, :notes, :category, :created_at, :updated_at)"
    );
    
    query.bindValue(":title", item.title());
    query.bindValue(":username", item.username());
    query.bindValue(":password", m_encryption->encrypt(item.password()));
    query.bindValue(":url", item.url());
    query.bindValue(":notes", item.notes());
    query.bindValue(":category", item.category());
    
    QDateTime now = QDateTime::currentDateTime();
    query.bindValue(":created_at", now);
    query.bindValue(":updated_at", now);
    
    if (!query.exec()) {
        qDebug() << "添加密码失败:" << query.lastError().text();
        return false;
    }
    
    // 设置生成的ID
    item.setId(query.lastInsertId().toInt());
    item.setCreatedAt(now);
    item.setUpdatedAt(now);
    
    return true;
}

bool Database::updatePassword(const PasswordItem &item)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return false;
    }
    
    QSqlQuery query;
    query.prepare(
        "UPDATE passwords SET "
        "title = :title, "
        "username = :username, "
        "password = :password, "
        "url = :url, "
        "notes = :notes, "
        "category = :category, "
        "updated_at = :updated_at "
        "WHERE id = :id"
    );
    
    query.bindValue(":id", item.id());
    query.bindValue(":title", item.title());
    query.bindValue(":username", item.username());
    query.bindValue(":password", m_encryption->encrypt(item.password()));
    query.bindValue(":url", item.url());
    query.bindValue(":notes", item.notes());
    query.bindValue(":category", item.category());
    query.bindValue(":updated_at", QDateTime::currentDateTime());
    
    if (!query.exec()) {
        qDebug() << "更新密码失败:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool Database::deletePassword(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM passwords WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "删除密码失败:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

PasswordItem Database::getPassword(int id)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return PasswordItem();
    }
    
    QSqlQuery query;
    query.prepare("SELECT * FROM passwords WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "获取密码失败:" << query.lastError().text();
        return PasswordItem();
    }
    
    PasswordItem item;
    item.setId(query.value("id").toInt());
    item.setTitle(query.value("title").toString());
    item.setUsername(query.value("username").toString());
    item.setPassword(m_encryption->decrypt(query.value("password").toString()));
    item.setUrl(query.value("url").toString());
    item.setNotes(query.value("notes").toString());
    item.setCategory(query.value("category").toString());
    item.setCreatedAt(query.value("created_at").toDateTime());
    item.setUpdatedAt(query.value("updated_at").toDateTime());
    
    return item;
}

QList<PasswordItem> Database::getAllPasswords()
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return QList<PasswordItem>();
    }
    
    QList<PasswordItem> passwords;
    QSqlQuery query("SELECT * FROM passwords ORDER BY title");
    
    while (query.next()) {
        PasswordItem item;
        item.setId(query.value("id").toInt());
        item.setTitle(query.value("title").toString());
        item.setUsername(query.value("username").toString());
        item.setPassword(m_encryption->decrypt(query.value("password").toString()));
        item.setUrl(query.value("url").toString());
        item.setNotes(query.value("notes").toString());
        item.setCategory(query.value("category").toString());
        item.setCreatedAt(query.value("created_at").toDateTime());
        item.setUpdatedAt(query.value("updated_at").toDateTime());
        
        passwords.append(item);
    }
    
    return passwords;
}

QList<PasswordItem> Database::searchPasswords(const QString &query)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return QList<PasswordItem>();
    }
    
    QList<PasswordItem> passwords;
    QSqlQuery sqlQuery;
    sqlQuery.prepare(
        "SELECT * FROM passwords WHERE "
        "title LIKE :query OR "
        "username LIKE :query OR "
        "url LIKE :query OR "
        "notes LIKE :query OR "
        "category LIKE :query "
        "ORDER BY title"
    );
    
    sqlQuery.bindValue(":query", "%" + query + "%");
    
    if (!sqlQuery.exec()) {
        qDebug() << "搜索密码失败:" << sqlQuery.lastError().text();
        return passwords;
    }
    
    while (sqlQuery.next()) {
        PasswordItem item;
        item.setId(sqlQuery.value("id").toInt());
        item.setTitle(sqlQuery.value("title").toString());
        item.setUsername(sqlQuery.value("username").toString());
        item.setPassword(m_encryption->decrypt(sqlQuery.value("password").toString()));
        item.setUrl(sqlQuery.value("url").toString());
        item.setNotes(sqlQuery.value("notes").toString());
        item.setCategory(sqlQuery.value("category").toString());
        item.setCreatedAt(sqlQuery.value("created_at").toDateTime());
        item.setUpdatedAt(sqlQuery.value("updated_at").toDateTime());
        
        passwords.append(item);
    }
    
    return passwords;
}

QStringList Database::getAllCategories()
{
    QStringList categories;
    QSqlQuery query("SELECT DISTINCT category FROM passwords WHERE category IS NOT NULL AND category != '' ORDER BY category");
    
    while (query.next()) {
        categories.append(query.value(0).toString());
    }
    
    return categories;
}

bool Database::importFromCsv(const QString &filename)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return false;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开CSV文件:" << filename;
        return false;
    }
    
    QTextStream in(&file);
    // 跳过标题行
    QString header = in.readLine();
    
    // 开始事务
    m_db.transaction();
    
    bool success = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        
        // 确保有足够的字段
        if (fields.size() < 3) {
            continue;
        }
        
        PasswordItem item;
        item.setTitle(fields.at(0));
        item.setUsername(fields.size() > 1 ? fields.at(1) : "");
        item.setPassword(fields.size() > 2 ? fields.at(2) : "");
        item.setUrl(fields.size() > 3 ? fields.at(3) : "");
        item.setNotes(fields.size() > 4 ? fields.at(4) : "");
        item.setCategory(fields.size() > 5 ? fields.at(5) : "");
        
        if (!addPassword(item)) {
            success = false;
            break;
        }
    }
    
    file.close();
    
    // 提交或回滚事务
    if (success) {
        m_db.commit();
    } else {
        m_db.rollback();
    }
    
    return success;
}

bool Database::exportToCsv(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法创建CSV文件:" << filename;
        return false;
    }
    
    QTextStream out(&file);
    
    // 写入标题行
    out << "标题,用户名,密码,网址,备注,分类\n";
    
    // 获取所有密码
    QList<PasswordItem> passwords = getAllPasswords();
    
    for (const PasswordItem &item : passwords) {
        out << item.title() << ","
            << item.username() << ","
            << item.password() << ","
            << item.url() << ","
            << item.notes() << ","
            << item.category() << "\n";
    }
    
    file.close();
    return true;
}

bool Database::importFromJson(const QString &filename)
{
    if (!m_encryption) {
        qDebug() << "加密模块未初始化";
        return false;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开JSON文件:" << filename;
        return false;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "JSON格式无效";
        return false;
    }
    
    QJsonArray passwordsArray = doc.array();
    
    // 开始事务
    m_db.transaction();
    
    bool success = true;
    for (const QJsonValue &value : passwordsArray) {
        if (!value.isObject()) {
            continue;
        }
        
        QJsonObject obj = value.toObject();
        PasswordItem item;
        item.setTitle(obj["title"].toString());
        item.setUsername(obj["username"].toString());
        item.setPassword(obj["password"].toString());
        item.setUrl(obj["url"].toString());
        item.setNotes(obj["notes"].toString());
        item.setCategory(obj["category"].toString());
        
        if (!addPassword(item)) {
            success = false;
            break;
        }
    }
    
    // 提交或回滚事务
    if (success) {
        m_db.commit();
    } else {
        m_db.rollback();
    }
    
    return success;
}

bool Database::exportToJson(const QString &filename)
{
    QList<PasswordItem> passwords = getAllPasswords();
    
    QJsonArray passwordsArray;
    for (const PasswordItem &item : passwords) {
        QJsonObject obj;
        obj["title"] = item.title();
        obj["username"] = item.username();
        obj["password"] = item.password();
        obj["url"] = item.url();
        obj["notes"] = item.notes();
        obj["category"] = item.category();
        
        passwordsArray.append(obj);
    }
    
    QJsonDocument doc(passwordsArray);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建JSON文件:" << filename;
        return false;
    }
    
    file.write(jsonData);
    file.close();
    
    return true;
}