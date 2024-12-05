#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "savesecretwindow.h"
#include "secretlistmodel.h"
#include "secretitemdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newSecret_clicked();
    void onEditButtonClicked(int index); // 成员函数槽，处理编辑操作
    void handleSaveSecretSuccess();  // 用于处理 SaveSecretSuccess 关闭事件并调用showList方法
    void on_textEditSearch_textChanged();

private:
    Ui::MainWindow *ui;
    AddSecretWindow *addSecretWindow;
    int editRowIndex; // 新增，用于记录修改的行号

private:
    SecretListModel *itemModel;
    void showList(const QString &searchKey);
    void showPageList(int pageIndex, int pageSize);
    void showSearchList(const QString &searchKey);
    void showSelectedSecret(const QModelIndex &index);
    void openAddSecretWindow(int index); // 打开 AddSecretWindow 的函数
    void refreshDisplayContent(const QModelIndex &index);
};
#endif // MAINWINDOW_H
