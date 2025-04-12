cmake_minimum_required(VERSION 3.16)
project(QtSecretTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Core Gui)

# 设置自动生成的文件目录
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置UI文件路径
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/database.cpp
    src/database.h
    src/passworditem.cpp
    src/passworditem.h
    src/encryption.cpp
    src/encryption.h
    src/importexport.cpp
    src/importexport.h
    src/addpassworddialog.cpp
    src/addpassworddialog.h
    src/editpassworddialog.cpp
    src/editpassworddialog.h
    ui/mainwindow.ui
    ui/addpassword.ui
    ui/editpassword.ui
)

qt_add_executable(QtSecretTool
    ${PROJECT_SOURCES}
)

target_link_libraries(QtSecretTool PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Core
    Qt6::Gui
)

# 安装
install(TARGETS QtSecretTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)