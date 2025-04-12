#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "database.h"
#include "encryption.h"
#include "importexport.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    // 文件菜单
    void on_actionNew_triggered();
    void on_actionImport_triggered();
    void on_actionExport_triggered();
    void on_actionExit_triggered();
    
    // 编辑菜单
    void on_actionAdd_triggered();
    void on_actionEdit_triggered();
    void on_actionDelete_triggered();
    void on_actionCopyUsername_triggered();
    void on_actionCopyPassword_triggered();
    
    // 帮助菜单
    void on_actionAbout_triggered();
    
    // 搜索
    void on_searchButton_clicked();
    void on_searchLineEdit_returnPressed();
    
    // 表格双击
    void on_passwordTableView_doubleClicked(const QModelIndex &index);
    
    // 分类树点击
    void on_categoryTreeView_clicked(const QModelIndex &index);
    
private:
    Ui::MainWindow *ui;
    Database *m_database;
    Encryption *m_encryption;
    ImportExport *m_importExport;
    QStandardItemModel *m_passwordModel;
    QStandardItemModel *m_categoryModel;
    QSortFilterProxyModel *m_proxyModel;
    
    // 初始化
    bool initialize();
    bool setupMasterPassword();
    
    // 刷新数据
    void refreshPasswordList();
    void refreshCategoryTree();
    
    // 获取选中的密码项
    PasswordItem getSelectedPassword();
    
    // 显示错误消息
    void showError(const QString &message);
};

#endif // MAINWINDOW_H