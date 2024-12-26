#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "database/DatabaseManager.h"
#include "config/settingmanager.h"

void cleanupFunction() {
    qDebug() << "Performing cleanup tasks before exiting...";
    // 你的关闭逻辑
    // 关闭数据库连接
    DatabaseManager::instance().closeDatabase();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序图标
    QIcon icon(":/icons/icon_yellow.png");
    app.setWindowIcon(icon);


    QString appDirPath = QCoreApplication::applicationDirPath();
    qDebug() << "应用程序二进制可执行文件所在路径: " << appDirPath;
    QString dbPath = SettingsManager::getInstance().value(dbPathKey).toString();
    qDebug() << "dbPath 1: " << dbPath;
    if (dbPath.isEmpty()) {
        dbPath = appDirPath;
    }
    qDebug() << "dbPath 2: " << dbPath;
    dbPath = dbPath + "/secret_tool.db";
    QString encryptionKey = "bunenggaosubieren";

    // 初始化数据库管理器单例并打开数据库连接
    if (!DatabaseManager::instance().openDatabase(dbPath, encryptionKey)) {
        return -1;
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qt-secret-tool_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        cleanupFunction();
    });


    return app.exec();
}
