#ifndef PASSWORDITEM_H
#define PASSWORDITEM_H

#include <QString>
#include <QDateTime>
#include <QObject>

/**
 * @brief 密码项类，表示单个密码条目
 * 
 * 该类存储密码相关的所有信息，包括标题、用户名、密码等
 */
class PasswordItem : public QObject
{
    Q_OBJECT
    
    // 定义属性，使其可在 QML 中访问
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt CONSTANT)
    Q_PROPERTY(QDateTime modifiedAt READ modifiedAt NOTIFY modifiedAtChanged)

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit PasswordItem(QObject *parent = nullptr);
    
    /**
     * @brief 带参数的构造函数
     * @param id 唯一标识符
     * @param title 标题
     * @param username 用户名
     * @param password 密码
     * @param url 网址
     * @param notes 备注
     * @param category 分类
     * @param parent 父对象指针
     */
    PasswordItem(const QString &id, 
                 const QString &title,
                 const QString &username,
                 const QString &password,
                 const QString &url = QString(),
                 const QString &notes = QString(),
                 const QString &category = QString(),
                 QObject *parent = nullptr);

    // Getters
    QString id() const;
    QString title() const;
    QString username() const;
    QString password() const;
    QString url() const;
    QString notes() const;
    QString category() const;
    QDateTime createdAt() const;
    QDateTime modifiedAt() const;

    // Setters
    void setTitle(const QString &title);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setUrl(const QString &url);
    void setNotes(const QString &notes);
    void setCategory(const QString &category);

signals:
    // 属性变更信号
    void titleChanged();
    void usernameChanged();
    void passwordChanged();
    void urlChanged();
    void notesChanged();
    void categoryChanged();
    void modifiedAtChanged();

private:
    QString m_id;          // 唯一标识符
    QString m_title;       // 标题
    QString m_username;    // 用户名
    QString m_password;    // 密码
    QString m_url;         // 网址
    QString m_notes;       // 备注
    QString m_category;    // 分类
    QDateTime m_createdAt; // 创建时间
    QDateTime m_modifiedAt; // 最后修改时间
    
    /**
     * @brief 更新修改时间
     */
    void updateModifiedTime();
};

#endif // PASSWORDITEM_H