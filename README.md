
# Qt Secret Tool

一个简单实用的密码管理工具，基于 Qt 框架开发。

## 功能特点

- 安全存储密码信息
- 便捷的密码管理（增删改查）
- 全文搜索功能
- 支持密码数据的导入导出
- 简洁直观的用户界面

## 技术栈

- C++
- Qt Quick (QML)
- SQLite (数据存储)
- OpenSSL (加密)

## 构建要求

- Qt 6.2+ 
- CMake 3.16+
- C++17 兼容的编译器

## 构建步骤

1. 克隆仓库
```bash
git clone https://github.com/FlyingonTemp/qt-secret-tool.git
cd qt-secret-tool
```

/Users/yuanzhaoyi/Develop/github.com/FlyingonTemp/qt-secret-tool/
├── CMakeLists.txt           # 项目构建配置
├── README.md                # 项目说明文档
├── src/                     # 源代码目录
│   ├── main.cpp             # 程序入口
│   ├── core/                # 核心功能
│   │   ├── passwordmanager.h    # 密码管理类头文件
│   │   ├── passwordmanager.cpp  # 密码管理类实现
│   │   ├── passworditem.h       # 密码项类头文件
│   │   ├── passworditem.cpp     # 密码项类实现
│   │   ├── encryption.h         # 加密工具类头文件
│   │   └── encryption.cpp       # 加密工具类实现
│   ├── io/                  # 导入导出功能
│   │   ├── filemanager.h        # 文件管理类头文件
│   │   └── filemanager.cpp      # 文件管理类实现
│   └── ui/                  # 用户界面
│       ├── qml/                 # QML 文件
│       │   ├── main.qml         # 主界面
│       │   ├── PasswordList.qml # 密码列表组件
│       │   ├── PasswordDetail.qml # 密码详情组件
│       │   ├── AddPassword.qml  # 添加密码组件
│       │   └── SearchBar.qml    # 搜索栏组件
│       └── resources.qrc        # 资源文件
└── assets/                  # 资源文件目录
    └── icons/               # 图标资源

### 命令
##### 执行 encryption_test.cpp
```
g++ -o encryption_test src/core/encryption_test.
cpp src/core/encryption.cpp -I. -std=c++17 $(pkg-config --cflags --libs Qt6Core)
./encryption_test
```