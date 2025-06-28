# VSCode Qt 开发环境配置说明

## 📁 配置文件概览

### 1. `.vscode/c_cpp_properties.json`
- **作用**: 配置IntelliSense代码智能提示
- **关键配置**:
  - 编译器路径: `H:/Qt/Tools/mingw1310_64/bin/gcc.exe`
  - 包含路径: Qt头文件目录 
  - 预定义宏: `QT_CORE_LIB`, `QT_GUI_LIB`, `QT_WIDGETS_LIB`

### 2. `.vscode/settings.json` 
- **作用**: VSCode工作区设置
- **关键配置**:
  - PATH环境变量: 包含Qt工具和MinGW编译器路径
  - 编译器和调试器路径设置
  - 文件关联设置(.pro, .qrc, .ui, .qss)

### 3. `.vscode/tasks.json`
- **作用**: 定义构建和运行任务
- **可用任务**:
  - `Quick Build with qmake`: 使用批处理脚本快速构建
  - `Build with qmake`: 生成Makefile
  - `Make project`: 编译项目 
  - `Run in Test Directory`: 在test目录下运行程序
  - `Build and Run in Test`: 构建并在test目录运行

### 4. `.vscode/launch.json`
- **作用**: 调试配置
- **关键配置**:
  - 程序路径: `${workspaceFolder}/release/PL0.exe`
  - 工作目录: `${workspaceFolder}/test` (重要: 程序在test目录运行)
  - 调试器: MinGW GDB

## 🚀 使用方法

### 构建项目
1. **方法1**: 按 `Ctrl+Shift+B` 使用默认构建任务
2. **方法2**: 按 `Ctrl+Shift+P` → "Tasks: Run Task" → 选择具体任务
3. **方法3**: 运行 `build_qt.bat` 或 `test_qt_setup.bat`

### 调试运行
1. **调试**: 按 `F5` 启动调试 (程序会在test目录运行)
2. **直接运行**: 使用 "Run in Test Directory" 任务

### 测试用例
- test目录包含大量`.PL0`测试文件
- 程序运行在test目录，可以直接读取测试文件
- 生成的`.COD`文件也会在test目录中

## 🛠️ 环境路径

- **Qt安装路径**: `H:/Qt/6.9.1/mingw_64/`
- **MinGW编译器**: `H:/Qt/Tools/mingw1310_64/bin/`
- **工作目录**: `${workspaceFolder}/test/` (用于读取测试用例)

## ✅ 功能特性

- ✅ Qt类和函数的代码补全
- ✅ 语法高亮和错误检测  
- ✅ 一键构建和调试
- ✅ 在test目录运行以读取测试用例
- ✅ 完整的调试支持
- ✅ 多种构建方式(qmake/make/批处理)

## 🔧 快捷键

- `Ctrl+Shift+B`: 构建项目
- `F5`: 开始调试
- `Ctrl+Shift+P` → "Tasks": 运行任务
- `Ctrl+` `: 打开终端

现在你的VSCode已经完全配置好Qt开发环境！
