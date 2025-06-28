@echo off
echo 使用Qt windeployqt工具部署应用...

REM 设置Qt环境
set PATH=H:\Qt\6.9.1\mingw_64\bin;H:\Qt\Tools\mingw1310_64\bin;%PATH%

REM 使用windeployqt自动部署
H:\Qt\6.9.1\mingw_64\bin\windeployqt.exe --debug --compiler-runtime release\PL0.exe

echo 部署完成！现在可以双击PL0.exe运行了。
pause
