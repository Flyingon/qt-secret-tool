#include "passworditem.h"

PasswordItem::PasswordItem()
    : m_id(-1)
{
}

PasswordItem::PasswordItem(int id, const QString &title, const QString &username, 
                           const QString &password, const QString &url, 
                           const QString &notes, const QString &category,
                           const QDateTime &createdAt, const QDateTime &updatedAt)
    : m_id(id)
    , m_title(title)
    , m_username(username)
    , m_password(password)
    , m_url(url)
    , m_notes(notes)
    , m_category(category)
    , m_createdAt(createdAt)
    , m_updatedAt(updatedAt)
{
}

int PasswordItem::id() const
{
    return m_id;
}

QString PasswordItem::title() const
{
    return m_title;
}

QString PasswordItem::username() const
{
    return m_username;
}

QString PasswordItem::password() const
{
    return m_password;
}

QString PasswordItem::url() const
{
    return m_url;
}

QString PasswordItem::notes() const
{
    return m_notes;
}

QString PasswordItem::category() const
{
    return m_category;
}

QDateTime PasswordItem::createdAt() const
{
    return m_createdAt;
}

QDateTime PasswordItem::updatedAt() const
{
    return m_updatedAt;
}

void PasswordItem::setId(int id)
{
    m_id = id;
}

void PasswordItem::setTitle(const QString &title)
{
    m_title = title;
}

void PasswordItem::setUsername(const QString &username)
{
    m_username = username;
}

void PasswordItem::setPassword(const QString &password)
{
    m_password = password;
}

void PasswordItem::setUrl(const QString &url)
{
    m_url = url;
}

void PasswordItem::setNotes(const QString &notes)
{
    m_notes = notes;
}

void PasswordItem::setCategory(const QString &category)
{
    m_category = category;
}

void PasswordItem::setCreatedAt(const QDateTime &createdAt)
{
    m_createdAt = createdAt;
}

void PasswordItem::setUpdatedAt(const QDateTime &updatedAt)
{
    m_updatedAt = updatedAt;
}