#include "passwordmanager.h"
#include "encryption.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>
#include <QUuid>

PasswordManager::PasswordManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_isFiltered(false)
{
    // 设置数据库路径
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // 初始化数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dataPath + "/passwords.db");
    
    // 确保数据库文件存在
    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError().text();
        return;
    }
    
    // 创建必要的表
    createTables();
}

PasswordManager::~PasswordManager()
{
    // 关闭数据库连接
    if (m_database.isOpen()) {
        m_database.close();
    }
    
    // 清理密码项
    qDeleteAll(m_passwords);
    m_passwords.clear();
    m_filteredPasswords.clear();
}

int PasswordManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    
    return m_isFiltered ? m_filteredPasswords.count() : m_passwords.count();
}

QVariant PasswordManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    int row = index.row();
    const QList<PasswordItem*> &items = m_isFiltered ? m_filteredPasswords : m_passwords;
    
    if (row < 0 || row >= items.count()) {
        return QVariant();
    }
    
    PasswordItem *item = items.at(row);
    
    switch (role) {
    case IdRole:
        return item->id();
    case TitleRole:
        return item->title();
    case UsernameRole:
        return item->username();
    case PasswordRole:
        return item->password();
    case UrlRole:
        return item->url();
    case NotesRole:
        return item->notes();
    case CategoryRole:
        return item->category();
    case CreatedAtRole:
        return item->createdAt();
    case ModifiedAtRole:
        return item->modifiedAt();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PasswordManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[UsernameRole] = "username";
    roles[PasswordRole] = "password";
    roles[UrlRole] = "url";
    roles[NotesRole] = "notes";
    roles[CategoryRole] = "category";
    roles[CreatedAtRole] = "createdAt";
    roles[ModifiedAtRole] = "modifiedAt";
    return roles;
}

int PasswordManager::count() const
{
    return m_isFiltered ? m_filteredPasswords.count() : m_passwords.count();
}

QString PasswordManager::searchText() const
{
    return m_searchText;
}

void PasswordManager::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        applyFilter();
        emit searchTextChanged();
    }
}

bool PasswordManager::isFiltered() const
{
    return m_isFiltered;
}

bool PasswordManager::initialize(const QString &masterPassword)
{
    if (masterPassword.isEmpty()) {
        qDebug() << "Master password cannot be empty";
        return false;
    }
    
    m_masterPassword = masterPassword;
    
    // 确保数据库已打开
    if (!m_database.isOpen()) {
        if (!m_database.open()) {
            qDebug() << "Failed to open database:" << m_database.lastError().text();
            return false;
        }
    }
    
    // 创建必要的表
    if (!createTables()) {
        qDebug() << "Failed to create tables";
        return false;
    }
    
    // 加载现有密码
    if (!loadPasswords()) {
        qDebug() << "Failed to load passwords";
        return false;
    }
    
    return true;
}

bool PasswordManager::validateMasterPassword(const QString &masterPassword)
{
    // 简单验证，实际应用中应该使用更安全的方法
    // 例如，存储主密码的哈希值并进行比较
    return masterPassword == m_masterPassword;
}

bool PasswordManager::changeMasterPassword(const QString &oldPassword, const QString &newPassword)
{
    if (!validateMasterPassword(oldPassword)) {
        return false;
    }
    
    // 重新加密所有密码项
    for (PasswordItem *item : m_passwords) {
        // 解密密码（使用旧密码）
        QByteArray decrypted = Encryption::decrypt(item->password().toUtf8(), oldPassword);
        
        // 重新加密（使用新密码）
        QByteArray encrypted = Encryption::encrypt(decrypted, newPassword);
        
        // 更新密码项
        item->setPassword(QString::fromUtf8(encrypted));
        
        // 保存到数据库
        if (!updatePasswordInDb(item)) {
            return false;
        }
    }
    
    m_masterPassword = newPassword;
    return true;
}

bool PasswordManager::addPassword(const QString &title,
                                 const QString &username,
                                 const QString &password,
                                 const QString &url,
                                 const QString &notes,
                                 const QString &category)
{
    // 加密密码
    qDebug() << "加密用的主密码:" << m_masterPassword;
    QByteArray encryptedPassword = Encryption::encrypt(password.toUtf8(), m_masterPassword);
    
    // 使用 Base64 编码加密后的数据，避免二进制数据存储问题
    QString encryptedPasswordStr = encryptedPassword.toBase64();
    
    qDebug() << "创建密码时，加密后的密码字符串(Base64):" << encryptedPasswordStr;
    qDebug() << "加密后的密码字符串长度:" << encryptedPasswordStr.length();
    qDebug() << "加密后的密码字节数组长度:" << encryptedPassword.length();
    
    // 创建新密码项
    PasswordItem *item = new PasswordItem(
        QUuid::createUuid().toString(),
        title,
        username,
        encryptedPasswordStr,
        url,
        notes,
        category,
        this
    );
    
    // 保存到数据库
    if (!savePasswordToDb(item)) {
        qDebug() << "Failed to save password to database";
        delete item;
        return false;
    }
    
    // 添加到模型
    beginInsertRows(QModelIndex(), m_passwords.count(), m_passwords.count());
    m_passwords.append(item);
    endInsertRows();
    
    // 如果有过滤，重新应用
    if (m_isFiltered) {
        applyFilter();
    }
    
    emit countChanged();
    return true;
}

bool PasswordManager::updatePassword(const QString &id,
                                    const QString &title,
                                    const QString &username,
                                    const QString &password,
                                    const QString &url,
                                    const QString &notes,
                                    const QString &category)
{
    // 查找密码项
    PasswordItem *item = nullptr;
    int index = -1;
    
    for (int i = 0; i < m_passwords.count(); ++i) {
        if (m_passwords.at(i)->id() == id) {
            item = m_passwords.at(i);
            index = i;
            break;
        }
    }
    
    if (!item) {
        return false;
    }
    
    // 加密新密码（如果提供）
    QByteArray encryptedPassword;
    if (!password.isEmpty()) {
        encryptedPassword = Encryption::encrypt(password.toUtf8(), m_masterPassword);
    }
    
    // 更新密码项
    item->setTitle(title);
    item->setUsername(username);
    if (!password.isEmpty()) {
        item->setPassword(QString::fromUtf8(encryptedPassword));
    }
    item->setUrl(url);
    item->setNotes(notes);
    item->setCategory(category);
    
    // 更新数据库
    if (!updatePasswordInDb(item)) {
        return false;
    }
    
    // 通知视图更新
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    
    // 如果有过滤，重新应用
    if (m_isFiltered) {
        applyFilter();
    }
    
    return true;
}

bool PasswordManager::deletePassword(const QString &id)
{
    // 查找密码项
    int index = -1;
    
    for (int i = 0; i < m_passwords.count(); ++i) {
        if (m_passwords.at(i)->id() == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return false;
    }
    
    // 从数据库删除
    if (!deletePasswordFromDb(id)) {
        return false;
    }
    
    // 从模型删除
    beginRemoveRows(QModelIndex(), index, index);
    PasswordItem *item = m_passwords.takeAt(index);
    delete item;
    endRemoveRows();
    
    // 如果有过滤，重新应用
    if (m_isFiltered) {
        applyFilter();
    }
    
    emit countChanged();
    return true;
}

PasswordItem* PasswordManager::getPassword(const QString &id)
{
    for (PasswordItem *item : m_passwords) {
        if (item->id() == id) {
            return item;
        }
    }
    
    return nullptr;
}

bool PasswordManager::exportPasswords(const QString &filePath, const QString &format)
{
    if (format.toLower() == "json") {
        return exportToJson(filePath);
    } else if (format.toLower() == "csv") {
        return exportToCsv(filePath);
    } else {
        return false;
    }
}

// 在 passwordmanager.cpp 中添加这些函数的声明

bool PasswordManager::exportToJson(const QString &filePath)
{
    QJsonArray passwordsArray;
    
    for (PasswordItem *item : m_passwords) {
        QJsonObject passwordObj;
        passwordObj["id"] = item->id();
        passwordObj["title"] = item->title();
        passwordObj["username"] = item->username();
        
        // 解密密码
        QByteArray decryptedPassword = Encryption::decrypt(item->password().toUtf8(), m_masterPassword);
        passwordObj["password"] = QString::fromUtf8(decryptedPassword);
        
        passwordObj["url"] = item->url();
        passwordObj["notes"] = item->notes();
        passwordObj["category"] = item->category();
        passwordObj["createdAt"] = item->createdAt().toString(Qt::ISODate);
        passwordObj["modifiedAt"] = item->modifiedAt().toString(Qt::ISODate);
        
        passwordsArray.append(passwordObj);
    }
    
    QJsonDocument doc(passwordsArray);
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool PasswordManager::exportToCsv(const QString &filePath)
{
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
    // 写入CSV头
    out << "ID,标题,用户名,密码,URL,备注,分类,创建时间,修改时间\n";
    
    // 写入数据
    for (PasswordItem *item : m_passwords) {
        // 解密密码
        QByteArray decryptedPassword = Encryption::decrypt(item->password().toUtf8(), m_masterPassword);
        
        out << item->id() << ","
            << item->title() << ","
            << item->username() << ","
            << QString::fromUtf8(decryptedPassword) << ","
            << item->url() << ","
            << item->notes() << ","
            << item->category() << ","
            << item->createdAt().toString(Qt::ISODate) << ","
            << item->modifiedAt().toString(Qt::ISODate) << "\n";
    }
    
    file.close();
    return true;
}

bool PasswordManager::importPasswords(const QString &filePath, const QString &format)
{
    if (format.toLower() == "json") {
        return importFromJson(filePath);
    } else if (format.toLower() == "csv") {
        return importFromCsv(filePath);
    } else {
        return false;
    }
}

bool PasswordManager::importFromJson(const QString &filePath)
{
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        return false;
    }
    
    QJsonArray passwordsArray = doc.array();
    
    beginResetModel();
    
    for (const QJsonValue &value : passwordsArray) {
        if (!value.isObject()) {
            continue;
        }
        
        QJsonObject obj = value.toObject();
        
        // 加密密码
        QByteArray encryptedPassword = Encryption::encrypt(
            obj["password"].toString().toUtf8(), 
            m_masterPassword
        );
        
        // 创建密码项
        PasswordItem *item = new PasswordItem(
            obj["id"].toString(),
            obj["title"].toString(),
            obj["username"].toString(),
            QString::fromUtf8(encryptedPassword),
            obj["url"].toString(),
            obj["notes"].toString(),
            obj["category"].toString(),
            this
        );
        
        // 保存到数据库
        if (!savePasswordToDb(item)) {
            delete item;
            continue;
        }
        
        m_passwords.append(item);
    }
    
    endResetModel();
    
    // 如果有过滤，重新应用
    if (m_isFiltered) {
        applyFilter();
    }
    
    emit countChanged();
    return true;
}

bool PasswordManager::importFromCsv(const QString &filePath)
{
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    
    // 跳过CSV头
    if (!in.atEnd()) {
        in.readLine();
    }
    
    beginResetModel();
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        
        // 确保有足够的字段
        if (fields.size() < 4) {
            continue;
        }
        
        // 加密密码
        QByteArray encryptedPassword = Encryption::encrypt(
            fields[3].toUtf8(), 
            m_masterPassword
        );
        
        // 创建密码项
        PasswordItem *item = new PasswordItem(
            fields[0], // ID
            fields[1], // 标题
            fields[2], // 用户名
            QString::fromUtf8(encryptedPassword),
            fields.size() > 4 ? fields[4] : QString(), // URL
            fields.size() > 5 ? fields[5] : QString(), // 备注
            fields.size() > 6 ? fields[6] : QString(), // 分类
            this
        );
        
        // 保存到数据库
        if (!savePasswordToDb(item)) {
            delete item;
            continue;
        }
        
        m_passwords.append(item);
    }
    
    file.close();
    endResetModel();
    
    // 如果有过滤，重新应用
    if (m_isFiltered) {
        applyFilter();
    }
    
    emit countChanged();
    return true;
}

QString PasswordManager::generatePassword(int length,
                                         bool includeUppercase,
                                         bool includeLowercase,
                                         bool includeNumbers,
                                         bool includeSpecial)
{
    return Encryption::generateRandomPassword(
        length,
        includeUppercase,
        includeLowercase,
        includeNumbers,
        includeSpecial
    );
}

bool PasswordManager::createTables()
{
    if (!m_database.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }
    
    QSqlQuery query;
    
    // 创建密码表
    if (!query.exec("CREATE TABLE IF NOT EXISTS passwords ("
                   "id TEXT PRIMARY KEY,"
                   "title TEXT NOT NULL,"
                   "username TEXT,"
                   "password TEXT NOT NULL,"
                   "url TEXT,"
                   "notes TEXT,"
                   "category TEXT,"
                   "created_at TEXT NOT NULL,"
                   "modified_at TEXT NOT NULL"
                   ")")) {
        qDebug() << "Failed to create passwords table:" << query.lastError().text();
        return false;
    }
    
    // 创建配置表
    if (!query.exec("CREATE TABLE IF NOT EXISTS config ("
                   "key TEXT PRIMARY KEY,"
                   "value TEXT NOT NULL"
                   ")")) {
        qDebug() << "Failed to create config table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool PasswordManager::loadPasswords()
{
    if (!m_database.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }
    
    QSqlQuery query;
    if (!query.exec("SELECT id, title, username, password, url, notes, category, created_at, modified_at FROM passwords")) {
        qDebug() << "Failed to load passwords:" << query.lastError().text();
        return false;
    }
    
    beginResetModel();
    
    // 清理现有密码
    qDeleteAll(m_passwords);
    m_passwords.clear();
    m_filteredPasswords.clear();
    
    // 加载密码
    while (query.next()) {
        QString id = query.value(0).toString();
        QString title = query.value(1).toString();
        QString username = query.value(2).toString();
        QString encryptedPassword = query.value(3).toString();
        
        // 打印从数据库读取的加密密码字符串
        qDebug() << "从数据库读取的密码项，ID:" << id;
        qDebug() << "标题:" << title;
        qDebug() << "加密的密码字符串:" << encryptedPassword;
        qDebug() << "加密的密码字符串长度:" << encryptedPassword.length();
        
        PasswordItem *item = new PasswordItem(
            id,
            title,
            username,
            encryptedPassword,
            query.value(4).toString(), // url
            query.value(5).toString(), // notes
            query.value(6).toString(), // category
            this
        );
        
        m_passwords.append(item);
    }
    
    endResetModel();
    emit countChanged();
    
    return true;
}

bool PasswordManager::savePasswordToDb(PasswordItem *item)
{
    if (!m_database.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO passwords (id, title, username, password, url, notes, category, created_at, modified_at) "
                 "VALUES (:id, :title, :username, :password, :url, :notes, :category, :created_at, :modified_at)");
    
    query.bindValue(":id", item->id());
    query.bindValue(":title", item->title());
    query.bindValue(":username", item->username());
    query.bindValue(":password", item->password());
    query.bindValue(":url", item->url());
    query.bindValue(":notes", item->notes());
    query.bindValue(":category", item->category());
    query.bindValue(":created_at", item->createdAt());
    query.bindValue(":modified_at", item->modifiedAt());
    
    if (!query.exec()) {
        qDebug() << "Failed to save password to database:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool PasswordManager::updatePasswordInDb(PasswordItem *item)
{
    QSqlQuery query;
    query.prepare("UPDATE passwords SET title = ?, username = ?, password = ?, url = ?, notes = ?, "
                 "category = ?, modified_at = ? WHERE id = ?");
    
    query.bindValue(0, item->title());
    query.bindValue(1, item->username());
    query.bindValue(2, item->password());
    query.bindValue(3, item->url());
    query.bindValue(4, item->notes());
    query.bindValue(5, item->category());
    query.bindValue(6, item->modifiedAt().toString(Qt::ISODate));
    query.bindValue(7, item->id());
    
    if (!query.exec()) {
        emit databaseError("更新密码失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool PasswordManager::deletePasswordFromDb(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM passwords WHERE id = ?");
    query.bindValue(0, id);
    
    if (!query.exec()) {
        emit databaseError("删除密码失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

void PasswordManager::applyFilter()
{
    if (m_searchText.isEmpty()) {
        if (m_isFiltered) {
            beginResetModel();
            m_filteredPasswords.clear();
            m_isFiltered = false;
            endResetModel();
            emit isFilteredChanged();
        }
        return;
    }
    
    beginResetModel();
    
    m_filteredPasswords.clear();
    
    // 搜索所有字段
    for (PasswordItem *item : m_passwords) {
        if (item->title().contains(m_searchText, Qt::CaseInsensitive) ||
            item->username().contains(m_searchText, Qt::CaseInsensitive) ||
            item->url().contains(m_searchText, Qt::CaseInsensitive) ||
            item->notes().contains(m_searchText, Qt::CaseInsensitive) ||
            item->category().contains(m_searchText, Qt::CaseInsensitive)) {
            m_filteredPasswords.append(item);
        }
    }
    
    m_isFiltered = true;
    endResetModel();
    
    emit isFilteredChanged();
}

void PasswordManager::decryptPassword(const QString &id)
{
    qDebug() << "开始解密密码，ID:" << id;
    
    // 查找密码项
    PasswordItem *item = nullptr;
    
    for (PasswordItem *p : m_passwords) {
        if (p->id() == id) {
            item = p;
            break;
        }
    }
    
    if (!item) {
        qDebug() << "错误：找不到指定的密码项";
        emit databaseError("找不到指定的密码项");
        return;
    }
    
    qDebug() << "找到密码项，标题:" << item->title();
    
    // 获取存储的加密密码
    QString encryptedPassword = item->password();
    qDebug() << "加密的密码(Base64):" << encryptedPassword;
    
    // 从 Base64 解码回二进制数据
    QByteArray encryptedData = QByteArray::fromBase64(encryptedPassword.toLatin1());
    qDebug() << "解码后的加密数据长度:" << encryptedData.length();
    
    // 解密密码
    QByteArray decryptedPassword = Encryption::decrypt(encryptedData, m_masterPassword);
    qDebug() << "解密后的密码:" << QString::fromUtf8(decryptedPassword);
    
    // 发出信号，通知QML密码已解密
    emit passwordDecrypted(QString::fromUtf8(decryptedPassword));
    qDebug() << "已发出 passwordDecrypted 信号";
}