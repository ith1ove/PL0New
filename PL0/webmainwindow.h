#ifndef WEBMAINWINDOW_H
#define WEBMAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

// Qt与Web界面的桥接类
class QtWebBridge : public QObject {
    Q_OBJECT
    
public:
    explicit QtWebBridge(QObject *parent = nullptr);
    
public slots:
    // 供JavaScript调用的方法
    QString compileCode(const QString &sourceCode, const QString &fileName);
    void saveFile(const QString &fileName, const QString &content);
    QString loadFile(const QString &fileName);
    
signals:
    // 向JavaScript发送信号
    void compileFinished(const QString &result);
    void fileOperationResult(bool success, const QString &message);

private:
    // PL0编译器相关方法（从原来的MainWindow移植）
    void initializeCompiler();
    QString runPL0Compiler(const QString &code, const QString &filename);
};

// 现代化Web界面主窗口
class WebMainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit WebMainWindow(QWidget *parent = nullptr);
    ~WebMainWindow();
    
private slots:
    void onPageLoaded();
    
private:
    void setupWebView();
    void setupWebChannel();
    
    QWebEngineView *webView;
    QWebChannel *webChannel;
    QtWebBridge *bridge;
};

#endif // WEBMAINWINDOW_H
