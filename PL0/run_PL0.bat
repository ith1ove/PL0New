@echo off
REM 设置Qt环境变量
set PATH=H:\Qt\6.9.1\mingw_64\bin;H:\Qt\Tools\mingw1310_64\bin;%PATH%

REM 切换到test目录（这样程序可以找到测试文件）
cd /d "%~dp0test"

REM 运行PL0程序
"%~dp0release\PL0.exe"

pause
