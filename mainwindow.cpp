#include "mainwindow.h"
#include <iostream>
#include "./ui_mainwindow.h"
#include "./savesecretwindow.h"
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
        onEditButtonClicked(index.row()); // 调用成员函数处理

    });

    QObject::connect(delegate, &SecretItemDelegate::deleteClicked, [&](const QModelIndex &index) {
        // 通过模型获取对应的key，以Qt::DisplayRole角色获取显示文本（这里假设是key）
        QVariant keyVariant = itemModel->data(index, Qt::DisplayRole);
        QString key = keyVariant.toString();

        if (!DatabaseManager::instance().deleteSecret(key)) {
            qDebug() << "删除密码记录失败";
            return;
        }
        // 删除操作
        itemModel->removeItem(index.row());

    });

    // 连接列表项点击信号到槽函数，用于显示对应secret
    QObject::connect(ui->listViewKey, &QListView::clicked, this, &MainWindow::showSelectedSecret);

    this->showList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showList() {

    std::vector<DBSecretItem> resultList;
    QString keyFilter = QString::fromStdString("");
    if (DatabaseManager::instance().querySecretList(keyFilter, 0, 100, resultList)) {
        itemModel->clearAllItems();
        for (const auto& item : resultList) {
            // qDebug() << "账号(key): " << item.key
            //          << ", 密码(secret): " << item.secret
            //          << ", 创建时间(created_at): " << item.createdAt.toString("yyyy-MM-dd hh:mm:ss")
            //          << ", 更新时间(updated_at): " << item.updatedAt.toString("yyyy-MM-dd hh:mm:ss");
            itemModel->addItem({item.key, item.secret});
        }
    } else {
        qDebug() << "查询密码记录列表失败";
    }
}

void MainWindow::showSelectedSecret(const QModelIndex &index)
{
    if (index.isValid()) {
        QVariant secretVariant = itemModel->data(index, Qt::UserRole);
        if (secretVariant.isValid()) {
            QString secret = secretVariant.toString();
            ui->textEditSecret->setText(secret);
        }
    }
}

void MainWindow::on_newSecret_clicked()
{
    std::cout << "click new Secret button" << std::endl;
    openAddSecretWindow(-1);
}

void MainWindow::onEditButtonClicked(int index) {
    openAddSecretWindow(index); // 调用成员函数，传递当前的 index
}

void MainWindow::openAddSecretWindow(int index) {
    qDebug() << "Secret Item: " << index;
    addSecretWindow = new AddSecretWindow();
    // 连接AddSecretWindow的saveSuccess信号到MainWindow的handleSaveSecretSuccess槽函数
    QObject::connect(addSecretWindow, &AddSecretWindow::saveSuccess, this, &MainWindow::handleSaveSecretSuccess);

    if (index >= 0) // 修改
    {
        // 先获取对应索引的QModelIndex对象
        QModelIndex modelIndex = itemModel->index(index, 0);
        if (modelIndex.isValid()) {
            // 通过模型获取对应的key，以Qt::DisplayRole角色获取显示文本（这里假设是key）
            QVariant keyVariant = itemModel->data(modelIndex, Qt::DisplayRole);
            QString key = keyVariant.toString();
            // 通过模型获取对应的secret，以Qt::UserRole角色获取自定义数据（这里假设是secret）
            QVariant secretVariant = itemModel->data(modelIndex, Qt::UserRole);
            QString secret = secretVariant.toString();
            // 使用获取到的key和secret构造SecretItem对象
            SecretItem item;
            item.key = key;
            item.secret = secret;
            addSecretWindow->SetToEdit(item);
        }
    }

    addSecretWindow->show();

}

// TODO 后面可以改成更新其中一行的展示
void MainWindow::handleSaveSecretSuccess()
{
    showList();
    // TODO 选中更新的这一行
}


