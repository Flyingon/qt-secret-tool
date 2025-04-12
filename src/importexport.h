#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <QObject>
#include "database.h"

class ImportExport : public QObject
{
    Q_OBJECT
    
public:
    explicit ImportExport(QObject *parent = nullptr);
    
    // 设置数据库
    void setDatabase(Database *database);
    
    // 导入导出CSV
    bool importFromCsv(const QString &filename);
    bool exportToCsv(const QString &filename);
    
    // 导入导出JSON
    bool importFromJson(const QString &filename);
    bool exportToJson(const QString &filename);
    
private:
    Database *m_database;
};

#endif // IMPORTEXPORT_H