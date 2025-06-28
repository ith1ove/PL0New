#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include "mainwindow.h"
#include "webmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("PL0 Compiler");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("广东工业大学");
    
    // 命令行参数解析
    QCommandLineParser parser;
    parser.setApplicationDescription("PL0编译器 - 支持传统Qt界面和现代Web界面");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption webModeOption(
        QStringList() << "w" << "web",
        "使用现代化Web界面（推荐）"
    );
    parser.addOption(webModeOption);
    
    QCommandLineOption classicModeOption(
        QStringList() << "c" << "classic",
        "使用传统Qt界面"
    );
    parser.addOption(classicModeOption);
    
    parser.process(app);
    
    // 检查是否有WebEngine支持
    bool hasWebEngine = true;
    #ifndef QT_WEBENGINE_LIB
    hasWebEngine = false;
    #endif
    
    // 决定使用哪种界面
    bool useWebInterface = false;
    
    if (parser.isSet(webModeOption)) {
        if (hasWebEngine) {
            useWebInterface = true;
        } else {
            QMessageBox::warning(nullptr, "警告", 
                "此版本不支持Web界面，将使用传统界面。\n"
                "要使用Web界面，请确保Qt WebEngine已安装。");
        }
    } else if (parser.isSet(classicModeOption)) {
        useWebInterface = false;
    } else {
        // 默认行为：如果有WebEngine支持，询问用户选择
        if (hasWebEngine) {
            int choice = QMessageBox::question(nullptr, 
                "选择界面模式",
                "请选择界面模式：\n\n"
                "• 现代化Web界面 - 使用Vue风格的现代化设计\n"
                "• 传统Qt界面 - 经典的Qt桌面界面\n\n"
                "推荐使用现代化Web界面获得更好的用户体验。",
                "现代化Web界面", "传统Qt界面", "", 0, 1);
            
            useWebInterface = (choice == 0);
        }
    }
    
    QMainWindow *mainWindow = nullptr;
    
    if (useWebInterface && hasWebEngine) {
        // 使用现代化Web界面
        #ifdef QT_WEBENGINE_LIB
        mainWindow = new WebMainWindow();
        #endif
    } else {
        // 使用传统Qt界面
        mainWindow = new MainWindow();
    }
    
    if (mainWindow) {
        mainWindow->show();
        
        // 设置窗口图标（如果有的话）
        // mainWindow->setWindowIcon(QIcon(":/icons/app.ico"));
        
        return app.exec();
    } else {
        QMessageBox::critical(nullptr, "错误", "无法创建主窗口");
        return -1;
    }
}
