#include "passworditem.h"
#include "encryption.h"
#include <QUuid>
#include <QByteArray>

PasswordItem::PasswordItem(QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_createdAt(QDateTime::currentDateTime())
    , m_modifiedAt(QDateTime::currentDateTime())
{
}

PasswordItem::PasswordItem(const QString &id, 
                           const QString &title,
                           const QString &username,
                           const QString &password,
                           const QString &url,
                           const QString &notes,
                           const QString &category,
                           QObject *parent)
    : QObject(parent)
    , m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id)
    , m_title(title)
    , m_username(username)
    , m_password(password)
    , m_url(url)
    , m_notes(notes)
    , m_category(category)
    , m_createdAt(QDateTime::currentDateTime())
    , m_modifiedAt(QDateTime::currentDateTime())
{
}

QString PasswordItem::id() const
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

QDateTime PasswordItem::modifiedAt() const
{
    return m_modifiedAt;
}

void PasswordItem::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        updateModifiedTime();
        emit titleChanged();
    }
}

void PasswordItem::setUsername(const QString &username)
{
    if (m_username != username) {
        m_username = username;
        updateModifiedTime();
        emit usernameChanged();
    }
}

void PasswordItem::setPassword(const QString &password)
{
    if (m_password != password) {
        m_password = password;
        updateModifiedTime();
        emit passwordChanged();
    }
}

void PasswordItem::setUrl(const QString &url)
{
    if (m_url != url) {
        m_url = url;
        updateModifiedTime();
        emit urlChanged();
    }
}

void PasswordItem::setNotes(const QString &notes)
{
    if (m_notes != notes) {
        m_notes = notes;
        updateModifiedTime();
        emit notesChanged();
    }
}

void PasswordItem::setCategory(const QString &category)
{
    if (m_category != category) {
        m_category = category;
        updateModifiedTime();
        emit categoryChanged();
    }
}

void PasswordItem::updateModifiedTime()
{
    m_modifiedAt = QDateTime::currentDateTime();
    emit modifiedAtChanged();
}