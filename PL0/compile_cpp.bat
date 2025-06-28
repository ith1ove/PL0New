@echo off
echo 使用g++直接编译PL0项目...

REM 设置编译器路径（请根据你的MinGW安装路径修改）
set GCC_PATH=H:\Qt\Tools\mingw1310_64\bin
set QT_PATH=H:\Qt\6.9.1\mingw_64

REM 添加到PATH
set PATH=%GCC_PATH%;%QT_PATH%\bin;%PATH%

echo 正在编译PL0项目...

REM Qt相关的包含路径和库路径
set QT_INCLUDES=-I"%QT_PATH%\include" -I"%QT_PATH%\include\QtCore" -I"%QT_PATH%\include\QtWidgets" -I"%QT_PATH%\include\QtGui"
set QT_LIBS=-L"%QT_PATH%\lib" -lQt6Core -lQt6Widgets -lQt6Gui

REM 编译命令
g++ -std=c++17 -O2 %QT_INCLUDES% -o PL0.exe main.cpp mainwindow.cpp PL0.cpp %QT_LIBS% -Wl,--subsystem,windows

if exist PL0.exe (
    echo 编译成功！生成可执行文件: PL0.exe
    echo 可以运行 PL0.exe 来启动程序
) else (
    echo 编译失败，请检查：
    echo 1. Qt和MinGW是否正确安装
    echo 2. 路径设置是否正确
    echo 3. 是否有语法错误
)

pause
