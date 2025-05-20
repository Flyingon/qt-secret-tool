#include "filemanager.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
}

QString FileManager::getSaveFilePath(const QString &title,
                                    const QString &directory,
                                    const QString &filter)
{
    QString dir = directory;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }
    
    return QFileDialog::getSaveFileName(nullptr, title, dir, filter);
}

QString FileManager::getOpenFilePath(const QString &title,
                                    const QString &directory,
                                    const QString &filter)
{
    QString dir = directory;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }
    
    return QFileDialog::getOpenFileName(nullptr, title, dir, filter);
}

QString FileManager::getFileExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

bool FileManager::fileExists(const QString &filePath)
{
    return QFileInfo::exists(filePath);
}