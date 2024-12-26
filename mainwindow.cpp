#include "mainwindow.h"
#include <iostream>


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

        // 添加确认弹框，询问用户是否确定删除
        QMessageBox msgBox;
        msgBox.setText("确定要删除该项密码记录吗？");
        msgBox.setInformativeText("此操作不可撤销，记录 " + key + " 将被永久删除。");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        // 根据用户选择决定是否执行删除操作
        if (ret == QMessageBox::Yes) {
            if (!DatabaseManager::instance().deleteSecret(key)) {
                qDebug() << "删除密码记录失败";
                return;
            }
            // 删除操作
            itemModel->removeItem(index.row());
        }

    });

    // 连接列表项点击信号到槽函数，用于显示对应secret
    QObject::connect(ui->listViewKey, &QListView::clicked, this, &MainWindow::showSelectedSecret);

    this->showList(QString());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showPageList(int pageIndex, int pageSize) {

    std::vector<DBSecretItem> resultList;
    QString keyFilter = QString::fromStdString("");
    if (DatabaseManager::instance().querySecretList(keyFilter, pageIndex, pageSize, resultList)) {
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

void MainWindow::showSearchList(const QString &searchKey) {

    std::vector<DBSecretItem> resultList;
    if (DatabaseManager::instance().fullTextSearch(searchKey, resultList)) {
        itemModel->clearAllItems();
        for (const auto& item : resultList) {
            // qDebug() << "账号(key): " << item.key
            //          << ", 密码(secret): " << item.secret
            //          << ", 创建时间(created_at): " << item.createdAt.toString("yyyy-MM-dd hh:mm:ss")
            //          << ", 更新时间(updated_at): " << item.updatedAt.toString("yyyy-MM-dd hh:mm:ss");
            itemModel->addItem({item.key, item.secret});
        }
    } else {
        qDebug() << "搜索密码记录列表失败";
    }
}

void MainWindow::showList(const QString &searchKey) {
    if (searchKey.isEmpty()) {
        showPageList(0, 1000);
        return;
    }
    showSearchList(searchKey);
    return;
}

void MainWindow::showSelectedSecret(const QModelIndex &index)
{
    this -> refreshDisplayContent(index);
}

// 用于根据索引刷新展示内容
void MainWindow::refreshDisplayContent(const QModelIndex &index) {
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
        this->editRowIndex = index; // 记录当前修改的行号
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
    int rowToSelect = this->editRowIndex; // 获取要选中的行号
    QString searchKey = ui->textEditSearch->toPlainText();
    showList(searchKey);
    if (rowToSelect >= 0 && rowToSelect < itemModel->rowCount()) {
        QModelIndex indexToSelect = itemModel->index(rowToSelect, 0);
        ui->listViewKey->setCurrentIndex(indexToSelect); // 选中该行
        this -> refreshDisplayContent(indexToSelect);  // 调用封装函数刷新展示内容
    }
}


void MainWindow::on_textEditSearch_textChanged()
{
    QString searchKey = ui->textEditSearch->toPlainText();
    showList(searchKey);
}


void MainWindow::on_pushButton_clicked()
{
    // 创建配置窗口实例
    ConfigWindow *configWindow = new ConfigWindow(this);
    // 以非模态窗口方式打开
    configWindow->show();
    return;
}

