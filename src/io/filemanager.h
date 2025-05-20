#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>

/**
 * @brief 文件管理类，处理文件操作
 * 
 * 该类提供文件选择、导入导出等功能
 */
class FileManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit FileManager(QObject *parent = nullptr);
    
    /**
     * @brief 选择保存文件
     * @param title 对话框标题
     * @param directory 初始目录
     * @param filter 文件过滤器
     * @return 选择的文件路径，如果取消则返回空字符串
     */
    Q_INVOKABLE QString getSaveFilePath(const QString &title,
                                       const QString &directory,
                                       const QString &filter);
    
    /**
     * @brief 选择打开文件
     * @param title 对话框标题
     * @param directory 初始目录
     * @param filter 文件过滤器
     * @return 选择的文件路径，如果取消则返回空字符串
     */
    Q_INVOKABLE QString getOpenFilePath(const QString &title,
                                       const QString &directory,
                                       const QString &filter);
    
    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 文件扩展名
     */
    Q_INVOKABLE QString getFileExtension(const QString &filePath);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 文件是否存在
     */
    Q_INVOKABLE bool fileExists(const QString &filePath);
};

#endif // FILEMANAGER_H