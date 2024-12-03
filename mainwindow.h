#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addsecretwindow.h"
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

private:
    Ui::MainWindow *ui;
    AddSecretWindow *addSecretWindow;

private:
    SecretListModel *itemModel;
    void showList();
    void openAddSecretWindow(int index); // 打开 AddSecretWindow 的函数
};
#endif // MAINWINDOW_H
