#ifndef PASSWORDITEM_H
#define PASSWORDITEM_H

#include <QString>
#include <QDateTime>

class PasswordItem
{
public:
    PasswordItem();
    PasswordItem(int id, const QString &title, const QString &username, 
                 const QString &password, const QString &url, 
                 const QString &notes, const QString &category,
                 const QDateTime &createdAt, const QDateTime &updatedAt);
    
    int id() const;
    QString title() const;
    QString username() const;
    QString password() const;
    QString url() const;
    QString notes() const;
    QString category() const;
    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    
    void setId(int id);
    void setTitle(const QString &title);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setUrl(const QString &url);
    void setNotes(const QString &notes);
    void setCategory(const QString &category);
    void setCreatedAt(const QDateTime &createdAt);
    void setUpdatedAt(const QDateTime &updatedAt);
    
private:
    int m_id;
    QString m_title;
    QString m_username;
    QString m_password;
    QString m_url;
    QString m_notes;
    QString m_category;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // PASSWORDITEM_H