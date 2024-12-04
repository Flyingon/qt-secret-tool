#ifndef SAVESECRETWINDOW_H
#define SAVESECRETWINDOW_H

#include <QWidget>
#include <QAbstractButton>
#include "secretlistmodel.h"

namespace Ui {
class AddSecretWindow;
}

class AddSecretWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddSecretWindow(QWidget *parent = nullptr);
    ~AddSecretWindow();

    void SetToEdit(const SecretItem& secretItem);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::AddSecretWindow *ui;

signals:
    void saveSuccess();  // 新增自定义信号，用于通知保存成功事件
};

#endif // SAVESECRETWINDOW_H
