#include "addsecretwindow.h"
#include "ui_addsecretwindow.h"
#include "./database/DatabaseManager.h"
#include <QMessageBox>
#include <QDebug>

AddSecretWindow::AddSecretWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddSecretWindow)
{
    ui->setupUi(this);
}

AddSecretWindow::~AddSecretWindow()
{
    delete ui;
}

void AddSecretWindow::on_buttonBox_accepted()
{
    if (ui->plainTextEdit->toPlainText().isEmpty() || ui->textEdit->toPlainText().isEmpty()) {
        QMessageBox::critical(nullptr, "操作失败", "请输入正确的内容。");
        return;
    }
    qDebug() << "click on_buttonBox_accepted" << ui->plainTextEdit->toPlainText() << ui->textEdit->toPlainText();
    QString errMsg;
    if(DatabaseManager::instance().addPassword(ui->plainTextEdit->toPlainText(), ui->textEdit->toPlainText(), errMsg)) {
        this->close();
        return;
    } else {
        QMessageBox::critical(nullptr, "操作失败", errMsg);

    }
}


void AddSecretWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    // 获取按钮的角色
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);

    // 判断按钮的角色
    if (role == QDialogButtonBox::RejectRole) {
        // 点击了取消按钮（RejectRole）
        qDebug() << "Clicked Cancel button";
        this->close();
    } else if (role == QDialogButtonBox::AcceptRole) {
        // 点击了确认按钮（AcceptRole）
        // qDebug() << "Clicked Ok button";
        // 可以触发 on_buttonBox_accepted() 手动处理确认操作
        // on_buttonBox_accepted();
    }
}

