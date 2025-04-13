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

# 添加 iOS 平台支持
if(IOS)
    # 设置 iOS 架构
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    
    # 设置 iOS 部署目标版本为较低的版本
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum iOS deployment version")
    
    # 明确设置 Xcode 属性中的部署目标版本
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0")
    
    # 添加 Info.plist
    set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/ios/Info.plist")
    
    # 设置开发团队ID（请替换为您的实际团队ID）
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "AB80E87513038AD315D403B8B9ACAC2C41F0EF2D" CACHE STRING "Development Team ID")
    
    # 设置应用图标
    set(MACOSX_BUNDLE_ICON_FILE AppIcon)
    set(APP_ICON_RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources/ios/AppIcon.xcassets")
    
    # 设置资源文件属性
    set_source_files_properties(${APP_ICON_RESOURCE_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    
    # 设置授权文件
    set(ENTITLEMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/ios/QtSecretTool.entitlements")
    
    # 添加 Info.plist 文件和授权配置
    set_target_properties(QtSecretTool PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${MACOSX_BUNDLE_INFO_PLIST}"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"   # 1=iPhone, 2=iPad
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        # 设置 Xcode 属性中的部署目标版本
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
        # 禁用自动生成的启动屏幕
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME ""
        # 添加授权文件配置
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${ENTITLEMENTS_FILE}"
        # 确保正确的代码签名
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        # 允许在调试时使用任何配置文件
        XCODE_ATTRIBUTE_CODE_SIGN_ALLOW_ENTITLEMENTS_MODIFICATION YES
        # 使用自动配置文件
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
    )
    
    # 添加资源文件
    target_sources(QtSecretTool PRIVATE
        ${APP_ICON_RESOURCE_PATH}
    )
endif()