#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QAbstractListModel>
#include "passworditem.h"

/**
 * @brief 密码管理类，管理所有密码项
 * 
 * 该类提供密码的增删改查功能，并负责数据的持久化存储
 */
class PasswordManager : public QAbstractListModel
{
    Q_OBJECT
    
    // 定义属性，使其可在 QML 中访问
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(bool isFiltered READ isFiltered NOTIFY isFilteredChanged)

public:
    // 模型角色枚举
    enum PasswordRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        UsernameRole,
        PasswordRole,
        UrlRole,
        NotesRole,
        CategoryRole,
        CreatedAtRole,
        ModifiedAtRole
    };
    
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit PasswordManager(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~PasswordManager();
    
    // QAbstractListModel 接口实现
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // 获取密码项数量
    int count() const;
    
    // 搜索相关
    QString searchText() const;
    void setSearchText(const QString &text);
    bool isFiltered() const;
    
    /**
     * @brief 初始化数据库
     * @param masterPassword 主密码
     * @return 是否成功初始化
     */
    Q_INVOKABLE bool initialize(const QString &masterPassword);
    
    /**
     * @brief 验证主密码
     * @param masterPassword 要验证的主密码
     * @return 是否验证成功
     */
    Q_INVOKABLE bool validateMasterPassword(const QString &masterPassword);
    
    /**
     * @brief 更改主密码
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 是否成功更改
     */
    Q_INVOKABLE bool changeMasterPassword(const QString &oldPassword, const QString &newPassword);
    
    /**
     * @brief 添加新密码项
     * @param title 标题
     * @param username 用户名
     * @param password 密码
     * @param url 网址
     * @param notes 备注
     * @param category 分类
     * @return 是否成功添加
     */
    Q_INVOKABLE bool addPassword(const QString &title,
                                const QString &username,
                                const QString &password,
                                const QString &url = QString(),
                                const QString &notes = QString(),
                                const QString &category = QString());
    
    /**
     * @brief 更新密码项
     * @param id 密码项ID
     * @param title 标题
     * @param username 用户名
     * @param password 密码
     * @param url 网址
     * @param notes 备注
     * @param category 分类
     * @return 是否成功更新
     */
    Q_INVOKABLE bool updatePassword(const QString &id,
                                   const QString &title,
                                   const QString &username,
                                   const QString &password,
                                   const QString &url = QString(),
                                   const QString &notes = QString(),
                                   const QString &category = QString());
    
    /**
     * @brief 删除密码项
     * @param id 要删除的密码项ID
     * @return 是否成功删除
     */
    Q_INVOKABLE bool deletePassword(const QString &id);
    
    /**
     * @brief 获取密码项
     * @param id 密码项ID
     * @return 密码项对象
     */
    Q_INVOKABLE PasswordItem* getPassword(const QString &id);
    
    /**
     * @brief 导出所有密码
     * @param filePath 导出文件路径
     * @param format 导出格式 (json, csv)
     * @return 是否成功导出
     */
    Q_INVOKABLE bool exportPasswords(const QString &filePath, const QString &format = "json");
    
    /**
     * @brief 导入密码
     * @param filePath 导入文件路径
     * @param format 导入格式 (json, csv)
     * @return 是否成功导入
     */
    Q_INVOKABLE bool importPasswords(const QString &filePath, const QString &format = "json");
    
    /**
     * @brief 生成随机密码
     * @param length 密码长度
     * @param includeUppercase 是否包含大写字母
     * @param includeLowercase 是否包含小写字母
     * @param includeNumbers 是否包含数字
     * @param includeSpecial 是否包含特殊字符
     * @return 生成的随机密码
     */
    Q_INVOKABLE QString generatePassword(int length = 16,
                                        bool includeUppercase = true,
                                        bool includeLowercase = true,
                                        bool includeNumbers = true,
                                        bool includeSpecial = true);

    /**
     * @brief 解密密码
     * @param id 密码项ID
     */
     Q_INVOKABLE void decryptPassword(const QString &id);

signals:
    // 信号
    void countChanged();
    void searchTextChanged();
    void isFilteredChanged();
    void databaseError(const QString &errorMessage);
    void passwordDecrypted(const QString &password);

// 在 private: 部分添加这些函数声明
private:
    QList<PasswordItem*> m_passwords;         // 所有密码项
    QList<PasswordItem*> m_filteredPasswords; // 过滤后的密码项
    QString m_searchText;                     // 搜索文本
    QSqlDatabase m_database;                  // 数据库连接
    QString m_masterPassword;                 // 主密码
    bool m_isFiltered;                        // 是否已过滤
    
    /**
     * @brief 创建数据库表
     * @return 是否成功创建
     */
    bool createTables();
    
    /**
     * @brief 加载所有密码
     * @return 是否成功加载
     */
    bool loadPasswords();
    
    /**
     * @brief 保存密码到数据库
     * @param item 要保存的密码项
     * @return 是否成功保存
     */
    bool savePasswordToDb(PasswordItem *item);
    
    /**
     * @brief 更新数据库中的密码
     * @param item 要更新的密码项
     * @return 是否成功更新
     */
    bool updatePasswordInDb(PasswordItem *item);
    
    /**
     * @brief 从数据库删除密码
     * @param id 要删除的密码项ID
     * @return 是否成功删除
     */
    bool deletePasswordFromDb(const QString &id);
    
    /**
     * @brief 应用搜索过滤
     */
    void applyFilter();
    
    /**
     * @brief 导出密码到JSON文件
     * @param filePath 文件路径
     * @return 是否成功导出
     */
    bool exportToJson(const QString &filePath);
    
    /**
     * @brief 导出密码到CSV文件
     * @param filePath 文件路径
     * @return 是否成功导出
     */
    bool exportToCsv(const QString &filePath);
    
    /**
     * @brief 从JSON文件导入密码
     * @param filePath 文件路径
     * @return 是否成功导入
     */
    bool importFromJson(const QString &filePath);
    
    /**
     * @brief 从CSV文件导入密码
     * @param filePath 文件路径
     * @return 是否成功导入
     */
    bool importFromCsv(const QString &filePath);
};

#endif // PASSWORDMANAGER_H