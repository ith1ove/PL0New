@echo off
REM PL0 项目代码格式化脚本

echo 正在格式化项目代码...

REM 检查 prettier 是否安装
where prettier >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 错误: Prettier 未安装
    echo 请运行: npm install -g prettier
    pause
    exit /b 1
)

REM 格式化 HTML, JSON, CSS/QSS 文件
echo 格式化 HTML, JSON, CSS 文件...
prettier --write "*.html" "*.json" "*.qss" ".vscode/*.json"

REM 检查 clang-format 是否可用
where clang-format >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo 格式化 C++ 文件...
    for %%f in (*.cpp *.h) do (
        if not "%%f"=="moc_*" if not "%%f"=="qrc_*" (
            clang-format -i -style=file "%%f"
        )
    )
) else (
    echo 警告: clang-format 未找到，跳过 C++ 文件格式化
)

echo 代码格式化完成！
pause
