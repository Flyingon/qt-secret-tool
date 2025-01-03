cmake_minimum_required(VERSION 3.16)

project(qt-secret-tool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Dbus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Dbus)

# 添加 SQLCipher 路径
set(SQLCIPHER_INCLUDE_DIR "/opt/homebrew/Cellar/sqlcipher/4.6.1/include/sqlcipher")
set(SQLCIPHER_LIB_DIR "/opt/homebrew/Cellar/sqlcipher/4.6.1/lib")
# 通过 CMake 指定 SQLCipher 头文件和库
include_directories(${SQLCIPHER_INCLUDE_DIR})
link_directories(${SQLCIPHER_LIB_DIR})
# 启用 SQLITE_HAS_CODEC
add_definitions(-DSQLITE_HAS_CODEC)

# 设置 SQLCipher 库
set(SQLCIPHER_LIB "sqlcipher")

set(TS_FILES qt-secret-tool_zh_CN.ts)

if(APPLE)
    # 设置应用程序图标（适用于Mac）
    set_source_files_properties(resources/icon_yellow.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-secret-tool
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        database/DatabaseManager.cpp database/DatabaseManager.h
        savesecretwindow.h savesecretwindow.cpp savesecretwindow.ui
        secretlistmodel.h secretlistmodel.cpp
        secretitemdelegate.h secretitemdelegate.cpp
        windows/config/configwindow.cpp
        config/settingmanager.cpp
        resources.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qt-secret-tool APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(qt-secret-tool SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qt-secret-tool
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

message("QT_DIR: ${QT_DIR}")

target_link_libraries(qt-secret-tool
        PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::DBus
        ${SQLCIPHER_LIB})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qt-secret-tool)
endif()
set_target_properties(qt-secret-tool PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qt-secret-tool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-secret-tool)
endif()
