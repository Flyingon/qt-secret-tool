#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer> // 添加 QTimer 头文件
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_database(new Database(this))
    , m_encryption(new Encryption())
    , m_importExport(new ImportExport(this))
    , m_passwordModel(new QStandardItemModel(this))
    , m_categoryModel(new QStandardItemModel(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    
    // 设置表格模型
    m_passwordModel->setHorizontalHeaderLabels({"ID", "标题", "用户名", "网址", "分类", "创建时间", "更新时间"});
    m_proxyModel->setSourceModel(m_passwordModel);
    ui->passwordTableView->setModel(m_proxyModel);
    ui->passwordTableView->hideColumn(0); // 隐藏ID列
    ui->passwordTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 设置分类树模型
    m_categoryModel->setHorizontalHeaderLabels({"分类"});
    ui->categoryTreeView->setModel(m_categoryModel);
    ui->categoryTreeView->setHeaderHidden(true);
    
    // 设置数据库和加密
    m_database->setEncryption(m_encryption);
    m_importExport->setDatabase(m_database);
    
    // 初始化
    if (!initialize()) {
        QMessageBox::critical(this, "错误", "初始化失败，应用将退出。");
        QTimer::singleShot(0, this, &QMainWindow::close);
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initialize()
{
    // 初始化数据库
    if (!m_database->initialize()) {
        return false;
    }
    
    // 设置主密码
    if (!setupMasterPassword()) {
        return false;
    }
    
    // 刷新数据
    refreshPasswordList();
    refreshCategoryTree();
    
    return true;
}

bool MainWindow::setupMasterPassword()
{
    // 这里简化处理，实际应用中应该检查是否已有主密码
    bool ok;
    QString password = QInputDialog::getText(this, "主密码", 
                                            "请输入主密码:", 
                                            QLineEdit::Password, 
                                            "", &ok);
    if (!ok || password.isEmpty()) {
        return false;
    }
    
    return m_encryption->setMasterPassword(password);
}

void MainWindow::refreshPasswordList()
{
    m_passwordModel->removeRows(0, m_passwordModel->rowCount());
    
    QList<PasswordItem> passwords = m_database->getAllPasswords();
    
    for (const PasswordItem &item : passwords) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(item.id())));
        row.append(new QStandardItem(item.title()));
        row.append(new QStandardItem(item.username()));
        row.append(new QStandardItem(item.url()));
        row.append(new QStandardItem(item.category()));
        row.append(new QStandardItem(item.createdAt().toString("yyyy-MM-dd hh:mm:ss")));
        row.append(new QStandardItem(item.updatedAt().toString("yyyy-MM-dd hh:mm:ss")));
        
        m_passwordModel->appendRow(row);
    }
}

void MainWindow::refreshCategoryTree()
{
    m_categoryModel->removeRows(0, m_categoryModel->rowCount());
    
    // 添加"全部"分类
    QStandardItem *allItem = new QStandardItem("全部");
    m_categoryModel->appendRow(allItem);
    
    // 添加其他分类
    QStringList categories = m_database->getAllCategories();
    for (const QString &category : categories) {
        QStandardItem *item = new QStandardItem(category);
        m_categoryModel->appendRow(item);
    }
}

PasswordItem MainWindow::getSelectedPassword()
{
    QModelIndexList selection = ui->passwordTableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return PasswordItem();
    }
    
    // 获取ID
    QModelIndex index = m_proxyModel->mapToSource(selection.first());
    int id = m_passwordModel->item(index.row(), 0)->text().toInt();
    
    return m_database->getPassword(id);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "错误", message);
}

// 文件菜单槽函数
void MainWindow::on_actionNew_triggered()
{
    // 创建新数据库
    if (QMessageBox::question(this, "新建数据库", 
                             "确定要创建新数据库吗？这将清除所有现有数据。",
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        // 实现新建数据库逻辑
        // ...
    }
}

void MainWindow::on_actionImport_triggered()
{
    QStringList filters;
    filters << "CSV文件 (*.csv)" << "JSON文件 (*.json)";
    
    QString filename = QFileDialog::getOpenFileName(this, "导入密码", 
                                                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                  filters.join(";;"));
    if (filename.isEmpty()) {
        return;
    }
    
    bool success = false;
    if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
        success = m_importExport->importFromCsv(filename);
    } else if (filename.endsWith(".json", Qt::CaseInsensitive)) {
        success = m_importExport->importFromJson(filename);
    }
    
    if (success) {
        QMessageBox::information(this, "导入成功", "密码导入成功。");
        refreshPasswordList();
        refreshCategoryTree();
    } else {
        showError("导入失败，请检查文件格式。");
    }
}

void MainWindow::on_actionExport_triggered()
{
    QStringList filters;
    filters << "CSV文件 (*.csv)" << "JSON文件 (*.json)";
    
    QString filename = QFileDialog::getSaveFileName(this, "导出密码", 
                                                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                  filters.join(";;"));
    if (filename.isEmpty()) {
        return;
    }
    
    bool success = false;
    if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
        success = m_importExport->exportToCsv(filename);
    } else if (filename.endsWith(".json", Qt::CaseInsensitive)) {
        success = m_importExport->exportToJson(filename);
    }
    
    if (success) {
        QMessageBox::information(this, "导出成功", "密码导出成功。");
    } else {
        showError("导出失败。");
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

// 编辑菜单槽函数
void MainWindow::on_actionAdd_triggered()
{
    // 创建添加密码对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加密码");
    
    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout;
    
    // 创建输入控件
    QLineEdit *titleLineEdit = new QLineEdit(&dialog);
    QLineEdit *usernameLineEdit = new QLineEdit(&dialog);
    QLineEdit *passwordLineEdit = new QLineEdit(&dialog);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *urlLineEdit = new QLineEdit(&dialog);
    QComboBox *categoryComboBox = new QComboBox(&dialog);
    categoryComboBox->setEditable(true);
    QTextEdit *notesTextEdit = new QTextEdit(&dialog);
    
    // 创建密码生成按钮
    QPushButton *generatePasswordButton = new QPushButton("生成", &dialog);
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(passwordLineEdit);
    passwordLayout->addWidget(generatePasswordButton);
    
    // 添加到表单
    formLayout->addRow("标题:", titleLineEdit);
    formLayout->addRow("用户名:", usernameLineEdit);
    formLayout->addRow("密码:", passwordLayout);
    formLayout->addRow("网址:", urlLineEdit);
    formLayout->addRow("分类:", categoryComboBox);
    formLayout->addRow("备注:", notesTextEdit);
    
    // 创建按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);
    
    // 填充分类下拉框
    categoryComboBox->addItems(m_database->getAllCategories());
    
    // 生成密码按钮
    connect(generatePasswordButton, &QPushButton::clicked, [=]() {
        QString password = Encryption::generateRandomPassword();
        passwordLineEdit->setText(password);
    });
    
    if (dialog.exec() == QDialog::Accepted) {
        PasswordItem item;
        item.setTitle(titleLineEdit->text());
        item.setUsername(usernameLineEdit->text());
        item.setPassword(passwordLineEdit->text());
        item.setUrl(urlLineEdit->text());
        item.setCategory(categoryComboBox->currentText());
        item.setNotes(notesTextEdit->toPlainText());
        
        if (m_database->addPassword(item)) {
            refreshPasswordList();
            refreshCategoryTree();
        } else {
            showError("添加密码失败。");
        }
    }
}

void MainWindow::on_actionEdit_triggered()
{
    PasswordItem item = getSelectedPassword();
    if (item.id() == -1) {
        showError("请先选择一个密码项。");
        return;
    }
    
    // 创建编辑密码对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑密码");
    
    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout;
    
    // 创建输入控件
    QLineEdit *titleLineEdit = new QLineEdit(&dialog);
    QLineEdit *usernameLineEdit = new QLineEdit(&dialog);
    QLineEdit *passwordLineEdit = new QLineEdit(&dialog);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *urlLineEdit = new QLineEdit(&dialog);
    QComboBox *categoryComboBox = new QComboBox(&dialog);
    categoryComboBox->setEditable(true);
    QTextEdit *notesTextEdit = new QTextEdit(&dialog);
    
    // 创建密码显示和生成按钮
    QPushButton *showPasswordButton = new QPushButton("显示", &dialog);
    showPasswordButton->setCheckable(true);
    QPushButton *generatePasswordButton = new QPushButton("生成", &dialog);
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(passwordLineEdit);
    passwordLayout->addWidget(showPasswordButton);
    passwordLayout->addWidget(generatePasswordButton);
    
    // 添加到表单
    formLayout->addRow("标题:", titleLineEdit);
    formLayout->addRow("用户名:", usernameLineEdit);
    formLayout->addRow("密码:", passwordLayout);
    formLayout->addRow("网址:", urlLineEdit);
    formLayout->addRow("分类:", categoryComboBox);
    formLayout->addRow("备注:", notesTextEdit);
    
    // 创建按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);
    
    // 填充表单
    titleLineEdit->setText(item.title());
    usernameLineEdit->setText(item.username());
    passwordLineEdit->setText(item.password());
    urlLineEdit->setText(item.url());
    notesTextEdit->setText(item.notes());
    
    // 填充分类下拉框
    categoryComboBox->addItems(m_database->getAllCategories());
    categoryComboBox->setCurrentText(item.category());
    
    // 显示密码按钮
    connect(showPasswordButton, &QPushButton::toggled, [=](bool checked) {
        passwordLineEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    
    // 生成密码按钮
    connect(generatePasswordButton, &QPushButton::clicked, [=]() {
        QString password = Encryption::generateRandomPassword();
        passwordLineEdit->setText(password);
    });
    
    if (dialog.exec() == QDialog::Accepted) {
        item.setTitle(titleLineEdit->text());
        item.setUsername(usernameLineEdit->text());
        item.setPassword(passwordLineEdit->text());
        item.setUrl(urlLineEdit->text());
        item.setCategory(categoryComboBox->currentText());
        item.setNotes(notesTextEdit->toPlainText());
        
        if (m_database->updatePassword(item)) {
            refreshPasswordList();
            refreshCategoryTree();
        } else {
            showError("更新密码失败。");
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    PasswordItem item = getSelectedPassword();
    if (item.id() == -1) {
        showError("请先选择一个密码项。");
        return;
    }
    
    if (QMessageBox::question(this, "删除密码", 
                             QString("确定要删除密码 \"%1\" 吗？").arg(item.title()),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_database->deletePassword(item.id())) {
            refreshPasswordList();
            refreshCategoryTree();
        } else {
            showError("删除密码失败。");
        }
    }
}

void MainWindow::on_actionCopyUsername_triggered()
{
    PasswordItem item = getSelectedPassword();
    if (item.id() == -1) {
        showError("请先选择一个密码项。");
        return;
    }
    
    QApplication::clipboard()->setText(item.username());
    statusBar()->showMessage("用户名已复制到剪贴板", 2000);
}

void MainWindow::on_actionCopyPassword_triggered()
{
    PasswordItem item = getSelectedPassword();
    if (item.id() == -1) {
        showError("请先选择一个密码项。");
        return;
    }
    
    QApplication::clipboard()->setText(item.password());
    statusBar()->showMessage("密码已复制到剪贴板", 2000);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于密码管理器", 
                      "密码管理器 v0.1\n\n"
                      "一个简单、安全的跨平台密码管理工具。\n"
                      "基于Qt 6.8开发。");
}

// 搜索功能
void MainWindow::on_searchButton_clicked()
{
    QString query = ui->searchLineEdit->text();
    if (query.isEmpty()) {
        refreshPasswordList();
    } else {
        m_passwordModel->removeRows(0, m_passwordModel->rowCount());
        
        QList<PasswordItem> passwords = m_database->searchPasswords(query);
        
        for (const PasswordItem &item : passwords) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(QString::number(item.id())));
            row.append(new QStandardItem(item.title()));
            row.append(new QStandardItem(item.username()));
            row.append(new QStandardItem(item.url()));
            row.append(new QStandardItem(item.category()));
            row.append(new QStandardItem(item.createdAt().toString("yyyy-MM-dd hh:mm:ss")));
            row.append(new QStandardItem(item.updatedAt().toString("yyyy-MM-dd hh:mm:ss")));
            
            m_passwordModel->appendRow(row);
        }
    }
}

void MainWindow::on_searchLineEdit_returnPressed()
{
    on_searchButton_clicked();
}

// 表格双击
void MainWindow::on_passwordTableView_doubleClicked(const QModelIndex &index)
{
    on_actionEdit_triggered();
}

// 分类树点击
void MainWindow::on_categoryTreeView_clicked(const QModelIndex &index)
{
    QString category = index.data().toString();
    
    if (category == "全部") {
        refreshPasswordList();
    } else {
        m_passwordModel->removeRows(0, m_passwordModel->rowCount());
        
        QList<PasswordItem> passwords = m_database->getAllPasswords();
        
        for (const PasswordItem &item : passwords) {
            if (item.category() == category) {
                QList<QStandardItem*> row;
                row.append(new QStandardItem(QString::number(item.id())));
                row.append(new QStandardItem(item.title()));
                row.append(new QStandardItem(item.username()));
                row.append(new QStandardItem(item.url()));
                row.append(new QStandardItem(item.category()));
                row.append(new QStandardItem(item.createdAt().toString("yyyy-MM-dd hh:mm:ss")));
                row.append(new QStandardItem(item.updatedAt().toString("yyyy-MM-dd hh:mm:ss")));
                
                m_passwordModel->appendRow(row);
            }
        }
    }
}