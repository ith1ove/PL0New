#include "webmainwindow.h"
#include <QApplication>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <cstring>
#include <iostream>

// 这里需要包含原来的PL0编译器代码
// 为了演示，我们先创建一个简化版本

QtWebBridge::QtWebBridge(QObject *parent) : QObject(parent) {
    initializeCompiler();
}

void QtWebBridge::initializeCompiler() {
    // 初始化PL0编译器（从原来的runClicked方法移植部分代码）
    // 这里可以移植原来的编译器初始化代码
}

QString QtWebBridge::compileCode(const QString &sourceCode, const QString &fileName) {
    // 模拟编译过程 - 在实际实现中，这里会调用真正的PL0编译器
    QString result;
    
    try {
        // 1. 保存源代码到临时文件
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        QString sourceFile = tempDir + "/" + fileName + ".PL0";
        
        QFile file(sourceFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << sourceCode;
            file.close();
            
            // 2. 编译代码（这里需要集成原来的编译逻辑）
            result += "=== 编译 PL0 ===\n";
            result += "源文件: " + sourceFile + "\n";
            
            // 在这里调用原来的编译器逻辑
            // QString compileResult = runPL0Compiler(sourceCode, fileName);
            // result += compileResult;
            
            // 3. 模拟成功编译
            result += "编译成功!\n";
            result += "~~~ 运行 PL0 ~~~\n";
            result += "程序执行完成\n";
            result += "~~~ 结束 PL0 ~~~\n";
            
        } else {
            result = "错误: 无法创建临时文件";
        }
    } catch (const std::exception &e) {
        result = "编译错误: " + QString(e.what());
    }
    
    emit compileFinished(result);
    return result;
}

void QtWebBridge::saveFile(const QString &fileName, const QString &content) {
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        "保存PL0文件",
        fileName + ".PL0",
        "PL0 Files (*.PL0);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
            emit fileOperationResult(true, "文件保存成功");
        } else {
            emit fileOperationResult(false, "无法保存文件");
        }
    }
}

QString QtWebBridge::loadFile(const QString &fileName) {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "加载PL0文件",
        fileName + ".PL0",
        "PL0 Files (*.PL0);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();
            emit fileOperationResult(true, "文件加载成功");
            return content;
        } else {
            emit fileOperationResult(false, "无法读取文件");
        }
    }
    
    return QString();
}

QString QtWebBridge::runPL0Compiler(const QString &code, const QString &filename) {
    // 在这里集成原来的PL0编译器代码
    // 可以移植 MainWindow::runClicked() 中的编译逻辑
    return "编译结果将在这里显示";
}

// WebMainWindow 实现
WebMainWindow::WebMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("广东工业大学 PL0编译器 - 现代化Web界面");
    setMinimumSize(1200, 800);
    
    setupWebView();
    setupWebChannel();
}

WebMainWindow::~WebMainWindow() {
    delete webView;
    delete webChannel;
    delete bridge;
}

void WebMainWindow::setupWebView() {
    webView = new QWebEngineView(this);
    setCentralWidget(webView);
    
    // 加载HTML界面
    QString htmlPath = QDir::currentPath() + "/web_interface.html";
    if (QFile::exists(htmlPath)) {
        webView->load(QUrl::fromLocalFile(htmlPath));
    } else {
        // 如果文件不存在，显示错误信息
        QString errorHtml = R"(
            <html>
            <head><title>错误</title></head>
            <body style="font-family: Arial; text-align: center; padding: 50px;">
                <h1 style="color: #f44336;">文件未找到</h1>
                <p>无法找到 web_interface.html 文件</p>
                <p>请确保文件位于: <code>)" + htmlPath + R"(</code></p>
            </body>
            </html>
        )";
        webView->setHtml(errorHtml);
    }
    
    connect(webView, &QWebEngineView::loadFinished, this, &WebMainWindow::onPageLoaded);
}

void WebMainWindow::setupWebChannel() {
    webChannel = new QWebChannel(this);
    bridge = new QtWebBridge(this);
    
    // 注册桥接对象，使其可以在JavaScript中访问
    webChannel->registerObject("qtBridge", bridge);
    
    // 将WebChannel设置到WebEngineView
    webView->page()->setWebChannel(webChannel);
}

void WebMainWindow::onPageLoaded() {
    // 页面加载完成后的处理
    qDebug() << "Web界面加载完成";
    
    // 可以在这里执行一些初始化的JavaScript代码
    webView->page()->runJavaScript(R"(
        // 确保qtBridge已经可用
        if (typeof qtBridge !== 'undefined') {
            console.log('Qt桥接对象已就绪');
            window.qtBridge = qtBridge;
        } else {
            console.log('等待Qt桥接对象...');
            setTimeout(() => {
                if (typeof qtBridge !== 'undefined') {
                    window.qtBridge = qtBridge;
                }
            }, 1000);
        }
    )");
}
