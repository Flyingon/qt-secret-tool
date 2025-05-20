cmake_minimum_required(VERSION 3.16)

project(qt-secret-tool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置自动生成 moc 文件
set(CMAKE_AUTOMOC ON)
# 设置自动处理 Qt 资源文件
set(CMAKE_AUTORCC ON)

# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql Widgets)

# 添加资源文件
set(RESOURCES
    src/ui/resources.qrc
)

# 源文件
set(SOURCES
    src/main.cpp
    src/core/passwordmanager.cpp
    src/core/passworditem.cpp
    src/core/encryption.cpp
    src/io/filemanager.cpp
)

# 头文件
set(HEADERS
    src/core/passwordmanager.h
    src/core/passworditem.h
    src/core/encryption.h
    src/io/filemanager.h
)

# 添加可执行文件
qt_add_executable(qt_secret_tool  # 将连字符改为下划线
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 设置 QML 导入路径
qt_add_qml_module(qt_secret_tool  # 这里也需要修改
    URI com.flyingontemp.secretool  # 修改为有效的QML模块URI
    VERSION 1.0
    QML_FILES
        src/ui/qml/main.qml
        src/ui/qml/PasswordList.qml
        src/ui/qml/PasswordDetail.qml
        src/ui/qml/AddPassword.qml
        src/ui/qml/SearchBar.qml
)

# 链接 Qt 库
target_link_libraries(qt_secret_tool  # 这里也需要修改
    PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Sql
    Qt6::Widgets
)

# 安装配置
install(TARGETS qt_secret_tool  # 这里也需要修改
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
