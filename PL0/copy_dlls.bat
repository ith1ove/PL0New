@echo off
echo 正在复制Qt DLL文件...

REM 复制Qt6核心库
copy "H:\Qt\6.9.1\mingw_64\bin\Qt6Core.dll" "release\" 2>nul
copy "H:\Qt\6.9.1\mingw_64\bin\Qt6Gui.dll" "release\" 2>nul
copy "H:\Qt\6.9.1\mingw_64\bin\Qt6Widgets.dll" "release\" 2>nul

REM 复制MinGW运行时库
copy "H:\Qt\Tools\mingw1310_64\bin\libgcc_s_seh-1.dll" "release\" 2>nul
copy "H:\Qt\Tools\mingw1310_64\bin\libstdc++-6.dll" "release\" 2>nul
copy "H:\Qt\Tools\mingw1310_64\bin\libwinpthread-1.dll" "release\" 2>nul

echo DLL文件复制完成！
echo 现在可以双击PL0.exe运行了。
pause
