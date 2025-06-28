@echo off
echo 正在编译PL0项目...
echo 注意：如果编译失败，请确保已安装Qt和MinGW

REM 设置Qt环境变量（请根据你的Qt安装路径修改）
set QT_DIR=H:\Qt\6.9.1\mingw_64
set PATH=%QT_DIR%\bin;H:\Qt\Tools\mingw1310_64\bin;%PATH%

echo 当前目录: %CD%
echo Qt路径: %QT_DIR%

REM 清理旧文件
if exist PL0.exe del PL0.exe
if exist Makefile del Makefile
if exist Makefile.Debug del Makefile.Debug
if exist Makefile.Release del Makefile.Release

REM 运行qmake生成Makefile
echo 正在运行qmake...
qmake PL0.pro -spec win32-g++

REM 编译项目
echo 正在编译...
mingw32-make

if exist debug\PL0.exe (
    echo 编译成功！可执行文件位于: debug\PL0.exe
    copy debug\PL0.exe .
    echo 已复制到当前目录: PL0.exe
) else (
    echo 编译失败，请检查错误信息
)

pause
