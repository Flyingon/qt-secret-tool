#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "core/passwordmanager.h"
#include "core/passworditem.h"
#include "io/filemanager.h"

int main(int argc, char *argv[])
{
    // 设置应用程序属性
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("Qt Secret Tool");
    app.setOrganizationName("FlyingonTemp");
    app.setOrganizationDomain("github.com/FlyingonTemp");
    
    // 创建QML引擎
    QQmlApplicationEngine engine;
    
    // 注册C++类型到QML
    // 修改注册的QML类型
    qmlRegisterType<PasswordItem>("com.flyingontemp.secretool", 1, 0, "PasswordItem");
    
    // 创建管理器实例
    PasswordManager *passwordManager = new PasswordManager(&app);
    FileManager *fileManager = new FileManager(&app);
    
    // 将管理器实例暴露给QML
    engine.rootContext()->setContextProperty("passwordManager", passwordManager);
    engine.rootContext()->setContextProperty("fileManager", fileManager);
    
    // 加载主QML文件
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    
    return app.exec();
}