#include "configwindow.h"

ConfigWindow::ConfigWindow(QWidget *parent)
        : QWidget(parent, Qt::Window) {
    setWindowTitle("配置窗口");
    // 设置默认大小 (例如 400x300)
    resize(400, 300);

    // 设置窗口为父窗口居中
    if (parent) {
        QPoint parentPos = parent->pos();
        int x = parentPos.x() + (parent->width() - this->width()) / 2;
        int y = parentPos.y() + (parent->height() - this->height()) / 2;
        move(x, y);
    }

    // 创建控件
    dbPathLabel = new QLabel("数据库路径:");
    dbPathLineEdit = new QLineEdit();
    dbPathButton = new QPushButton("选择数据库路径");

    saveButton = new QPushButton("保存配置");

    // 设置布局
    QHBoxLayout *dbPathLayout = new QHBoxLayout();
    dbPathLayout->addWidget(dbPathLabel);
    dbPathLayout->addWidget(dbPathLineEdit);
    dbPathLayout->addWidget(dbPathButton);

//    QHBoxLayout *keyLayout = new QHBoxLayout();
//    keyLayout->addWidget(keyLabel);
//    keyLayout->addWidget(keyLineEdit);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(dbPathLayout);
    layout->addWidget(saveButton);

    setLayout(layout);

    // 连接信号和槽
    connect(dbPathButton, &QPushButton::clicked, this, &ConfigWindow::onSelectDatabasePath);
    connect(saveButton, &QPushButton::clicked, this, &ConfigWindow::onSaveConfig);

    // 加载已有配置
    loadConfig();
}

void ConfigWindow::onSelectDatabasePath() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择数据库路径");
    if (!dirPath.isEmpty()) {
        dbPathLineEdit->setText(dirPath);
    }
}

void ConfigWindow::onSaveConfig() {
    QString dbPath = dbPathLineEdit->text();

    // dbPath 不输入则删除，用回默认值
    if (dbPath.isEmpty()) {
        SettingsManager::getInstance().remove(dbPathKey);
    } else {
        // 使用 QSettings 保存配置
        QSettings settings("MyApp", "Config");
        SettingsManager::getInstance().setValue(dbPathKey, dbPath);
    }

    QMessageBox::information(this, "配置保存", "配置已保存成功！重启生效！");
    this->close();
}

void ConfigWindow::loadConfig() {
    QSettings settings("MyApp", "Config");
    QString dbPath = SettingsManager::getInstance().value(dbPathKey).toString();
    dbPathLineEdit->setText(dbPath);
}