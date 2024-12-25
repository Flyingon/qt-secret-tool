#include "configwindow.h"

ConfigWindow::ConfigWindow(QWidget *parent)
        : QWidget(parent, Qt::Window)
{
    setWindowTitle("配置窗口");
    // 设置默认大小 (例如 400x300)
    resize(400, 300);

    // 设置窗口为父窗口居中
    if (parent) {
        // 获取父窗口的中心位置
        QPoint parentCenter = parent->rect().center();
        // 计算当前窗口的大小
        QSize windowSize = size();
        // 设置当前窗口的位置，使其居中于父窗口
        move(parentCenter - QPoint(windowSize.width() / 2, windowSize.height() / 2));
    }

    // 创建控件
    dbPathLabel = new QLabel("数据库路径:");
    dbPathLineEdit = new QLineEdit();
    dbPathButton = new QPushButton("选择数据库路径");

    keyLabel = new QLabel("密钥:");
    keyLineEdit = new QLineEdit();

    saveButton = new QPushButton("保存配置");

    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(dbPathLabel);
    layout->addWidget(dbPathLineEdit);
    layout->addWidget(dbPathButton);
    layout->addWidget(keyLabel);
    layout->addWidget(keyLineEdit);
    layout->addWidget(saveButton);

    setLayout(layout);

    // 连接信号和槽
    connect(dbPathButton, &QPushButton::clicked, this, &ConfigWindow::onSelectDatabasePath);
    connect(saveButton, &QPushButton::clicked, this, &ConfigWindow::onSaveConfig);

    // 加载已有配置
    loadConfig();
}

void ConfigWindow::onSelectDatabasePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择数据库文件", "", "数据库文件 (*.db)");
    if (!filePath.isEmpty()) {
        dbPathLineEdit->setText(filePath);
    }
}

void ConfigWindow::onSaveConfig()
{
    QString dbPath = dbPathLineEdit->text();
    QString key = keyLineEdit->text();

    // 校验输入
    if (dbPath.isEmpty() || key.isEmpty()) {
        QMessageBox::warning(this, "输入无效", "请填写完整的配置。");
        return;
    }

    // 使用 QSettings 保存配置
    QSettings settings("MyApp", "Config");
    settings.setValue("databasePath", dbPath);
    settings.setValue("key", key);

    QMessageBox::information(this, "配置保存", "配置已保存成功！");
}

void ConfigWindow::loadConfig()
{
    QSettings settings("MyApp", "Config");

    QString dbPath = settings.value("databasePath").toString();
    QString key = settings.value("key").toString();

    dbPathLineEdit->setText(dbPath);
    keyLineEdit->setText(key);
}
