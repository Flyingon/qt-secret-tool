#include "mainwindow.h"
#include <iostream>
#include "./ui_mainwindow.h"
#include "./ui_addsecretwindow.h"
#include "./addsecretwindow.h"
#include "./database/DatabaseManager.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    itemModel = new SecretListModel();
    SecretItemDelegate *delegate = new SecretItemDelegate();
    ui->listViewKey->setModel(itemModel);
    ui->listViewKey->setItemDelegate(delegate);
    QObject::connect(delegate, &SecretItemDelegate::editClicked, [&](const QModelIndex &index) {
        // 编辑操作
        qDebug() << "Edit clicked:" << index.row();
    });

    QObject::connect(delegate, &SecretItemDelegate::deleteClicked, [&](const QModelIndex &index) {
        // 删除操作
        itemModel->removeItem(index.row());
    });

    this->showList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showList() {
    itemModel->addItem({"Secret 1", "Details 1"});
    itemModel->addItem({"Secret 2", "Details 2"});
}

void MainWindow::on_newSecret_clicked()
{
    std::cout << "click new Secret button" << std::endl;
    addSecretWindow = new AddSecretWindow();
    addSecretWindow->show();
}

