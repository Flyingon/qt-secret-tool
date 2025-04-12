#include "importexport.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>

ImportExport::ImportExport(QObject *parent)
    : QObject(parent)
    , m_database(nullptr)
{
}

void ImportExport::setDatabase(Database *database)
{
    m_database = database;
}

bool ImportExport::importFromCsv(const QString &filename)
{
    if (!m_database) {
        qDebug() << "数据库未初始化";
        return false;
    }
    
    return m_database->importFromCsv(filename);
}

bool ImportExport::exportToCsv(const QString &filename)
{
    if (!m_database) {
        qDebug() << "数据库未初始化";
        return false;
    }
    
    return m_database->exportToCsv(filename);
}

bool ImportExport::importFromJson(const QString &filename)
{
    if (!m_database) {
        qDebug() << "数据库未初始化";
        return false;
    }
    
    return m_database->importFromJson(filename);
}

bool ImportExport::exportToJson(const QString &filename)
{
    if (!m_database) {
        qDebug() << "数据库未初始化";
        return false;
    }
    
    return m_database->exportToJson(filename);
}