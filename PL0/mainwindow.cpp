#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QFontDatabase>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTabBar>
#include <QButtonGroup>
#include <QStandardPaths>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent){
    setWindowTitle(tr("广东工业大学 PL0编译器"));   //设置标题

    // 加载现代化样式
    loadModernStyle();
    
    createPL0Page();
    setCentralWidget(pl0Page);
}

MainWindow::~MainWindow(){
}

void MainWindow::createPL0Page(){
    pl0Page = new QWidget;

    // 定义页面字体样式
    QFont qfont;
    qfont.setPointSize(14);
    qfont.setFamily("黑体");
    qfont.setBold(true);

    QFont qfont1;
    qfont1.setPointSize(14);
    qfont1.setFamily("Courier New");
    qfont1.setBold(true);



    // 左部文本框
    pl0TextEdit = new QTextEdit();
    pl0TextEdit->setText(tr("***** PL/0 Compiler Demo *****"));
    pl0TextEdit->setFont(qfont1);
    pl0TextEdit->setToolTip(tr("编译和运行输出区域")); // 添加工具提示

    // 右部
    sourceLabel = new QLabel(tr("源程序名"));
    sourceLabel->setFont(qfont);

    sourceLineEdit = new QLineEdit(tr("EX18"));
    sourceLineEdit->setFont(qfont1);
    sourceLineEdit->setToolTip(tr("输入要编译的PL0程序文件名（不含扩展名）")); // 添加工具提示

    // 创建目标代码选项区域，使用按钮组模拟垂直Tab效果
    codeOptionsGroupBox = new QGroupBox(tr("目标代码"));
    
    // 创建垂直按钮组来替代Tab Bar
    QPushButton *showButton = new QPushButton(tr("显示生成的目标代码"));
    QPushButton *hideButton = new QPushButton(tr("仅显示源代码和输出"));
    
    // 设置按钮为可切换状态
    showButton->setCheckable(true);
    hideButton->setCheckable(true);
    showButton->setChecked(true); // 默认选择"显示"
    
    // 设置按钮样式和字体
    showButton->setFont(qfont);
    hideButton->setFont(qfont);
    showButton->setObjectName("tabButton");
    hideButton->setObjectName("tabButton");
    
    // 添加工具提示
    showButton->setToolTip(tr("显示生成的目标代码"));
    hideButton->setToolTip(tr("隐藏生成的目标代码"));
    
    // 创建按钮组确保互斥选择
    QButtonGroup *codeVisibilityButtonGroup = new QButtonGroup(this);
    codeVisibilityButtonGroup->addButton(showButton, 0);
    codeVisibilityButtonGroup->addButton(hideButton, 1);
    codeVisibilityButtonGroup->setExclusive(true);
    
    // 保存按钮组的引用以便后续使用
    codeVisibilityTabBar = nullptr; // 不再使用Tab Bar
    this->codeVisibilityButtonGroup = codeVisibilityButtonGroup; // 需要在头文件中添加此成员
    
    QLayout *codeOptionsLayout = new QVBoxLayout;
    codeOptionsLayout->addWidget(showButton);
    codeOptionsLayout->addWidget(hideButton);
    codeOptionsGroupBox->setLayout(codeOptionsLayout);
    codeOptionsGroupBox->setFont(qfont);

    runButton = new QPushButton(tr("RUN"));
    connect(runButton, SIGNAL(clicked(bool)), this, SLOT(runClicked()));
    runButton->setFont(qfont);
    runButton->setToolTip(tr("编译并运行PL0程序")); // 添加工具提示
    
    clearButton = new QPushButton(tr("清空"), this);
    clearButton->setObjectName("clearButton"); // 设置对象名以应用特殊样式
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearOutput()));
    clearButton->setFont(qfont);
    clearButton->setToolTip(tr("清空输出区域内容")); // 添加工具提示

    // 个人班级、学号、姓名部分
    // 定义页面字体样式
    QFont idFont;
    idFont.setPointSize(10);
    idFont.setFamily("黑体");
    idFont.setBold(true);
    idGroupBox = new QGroupBox(tr("作者 曾繁林"));
    classLabel = new QLabel(tr("班级 信息安全2班"));
    noLabel = new QLabel(tr("学号 3122004715"));
    nameLabel = new QLabel(tr(" "));
    QLayout *idLayout = new QVBoxLayout;
    idLayout->addWidget(classLabel);
    idLayout->addWidget(noLabel);
    idLayout->addWidget(nameLabel);
    idGroupBox->setLayout(idLayout);
    idGroupBox->setFont(idFont);

    QGridLayout *pageLayout = new QGridLayout;
    pageLayout->setColumnStretch(0,5);
    pageLayout->setColumnStretch(1,1);
    pageLayout->setRowStretch(0,1);     // label
    pageLayout->setRowStretch(1,1);     // lineedit
    pageLayout->setRowStretch(2,1);
    pageLayout->setRowStretch(3,5);     // groupbox
    pageLayout->setRowStretch(4,1);
    pageLayout->setRowStretch(5,2);     // button
    pageLayout->setRowStretch(6,3);
    pageLayout->setRowStretch(7,4);     // idgroupbox
    pageLayout->setRowStretch(8,3);
    pageLayout->addWidget(pl0TextEdit,0,0,9,1);
    pageLayout->addWidget(sourceLabel,0,1,1,1);
    pageLayout->addWidget(sourceLineEdit,1,1,1,1);
    pageLayout->addWidget(codeOptionsGroupBox,3,1,1,1);
    pageLayout->addWidget(runButton,5,1,1,1);
    pageLayout->addWidget(clearButton,6,1,1,1);
    pageLayout->addWidget(idGroupBox,7,1,1,1);
    pl0Page->setLayout(pageLayout);
}

void MainWindow::pl0printf(QString s){
    pl0TextEdit->moveCursor(QTextCursor::End);
    pl0TextEdit->insertPlainText("\n"+s);
}

void MainWindow::printfs(const char* s){
    QString qs=QString(s);
    pl0printf(qs);
}
void MainWindow::printcs(const char c){
    QString qs=QString(c);
    pl0printf(qs);
}
void MainWindow::printls(const char *s0, long v){
    QString qs=QString(s0)+QString::number(v);
    pl0printf(qs);
}
void MainWindow::printrs(const char *s0, float v){
    QString qs=QString(s0)+QString("%1").arg(0);
    pl0printf(qs);
}

void MainWindow::clearOutput() {
    pl0TextEdit->clear();
}

void MainWindow::loadModernStyle() {
    // 尝试加载QSS样式文件
    QFile styleFile("modern_style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(styleFile.readAll());
        qApp->setStyleSheet(style);
        styleFile.close();
    } else {
        // 如果文件不存在，使用内置样式
        applyBuiltinStyle();
    }
}

void MainWindow::applyBuiltinStyle() {
    // 内置现代化样式
    QString style = R"(
        /* 主窗口 */
        QMainWindow {
            background-color: #f5f5f5;
            color: #333333;
        }
        
        /* 文本编辑区域 */
        QTextEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: 1px solid #3c3c3c;
            border-radius: 8px;
            padding: 12px;
            font-family: "Consolas", "Monaco", "Courier New", monospace;
            font-size: 14px;
        }
        
        /* 输入框 */
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            color: #333333;
        }
        
        QLineEdit:focus {
            border-color: #2196F3;
            background-color: #fafafa;
        }
        
        /* 按钮 */
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: bold;
            font-size: 14px;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background-color: #1976D2;
        }
        
        QPushButton:pressed {
            background-color: #1565C0;
        }
        
        /* 清空按钮特殊样式 */
        #clearButton {
            background-color: #FF5722;
        }
        
        #clearButton:hover {
            background-color: #E64A19;
        }
        
        /* 分组框 */
        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #e0e0e0;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
            font-weight: bold;
            color: #424242;
        }
        
        QGroupBox::title {
            left: 15px;
            padding: 0 8px 0 8px;
            background-color: #ffffff;
            color: #2196F3;
        }
        
        /* Tab样式的按钮 - 垂直排列且文字水平 */
        QPushButton[objectName="tabButton"] {
            background-color: #e0e0e0;
            border: 1px solid #bdbdbd;
            border-radius: 4px;
            padding: 8px 12px;
            margin: 2px 0px;
            color: #424242;
            font-weight: 500;
            text-align: center;
        }
        
        QPushButton[objectName="tabButton"]:checked {
            background-color: #2196F3;
            color: white;
            border-color: #1976D2;
        }
        
        QPushButton[objectName="tabButton"]:hover {
            background-color: #f0f0f0;
            border-color: #9e9e9e;
        }
        
        QPushButton[objectName="tabButton"]:checked:hover {
            background-color: #1976D2;
        }
        
        /* 单选按钮 */
        QRadioButton {
            color: #424242;
            font-size: 10px;
        }
        
        /* 标签 */
        QLabel {
            color: #424242;
            font-weight: 500;
        }
    )";
    
    qApp->setStyleSheet(style);
}

// 添加自动查找test目录的函数
QString MainWindow::findTestDirectory() {
    // 获取当前工作目录
    QString currentPath = QDir::currentPath();
    QDir currentDir(currentPath);
    
    // 首先检查当前目录下是否有test文件夹
    if (currentDir.exists("test")) {
        return currentDir.absoluteFilePath("test");
    }
    
    // 检查上级目录
    if (currentDir.cdUp()) {
        if (currentDir.exists("test")) {
            return currentDir.absoluteFilePath("test");
        }
    }
    
    // 检查应用程序所在目录
    QString appPath = QCoreApplication::applicationDirPath();
    QDir appDir(appPath);
    if (appDir.exists("test")) {
        return appDir.absoluteFilePath("test");
    }
    
    // 检查应用程序上级目录
    if (appDir.cdUp()) {
        if (appDir.exists("test")) {
            return appDir.absoluteFilePath("test");
        }
    }
    
    // 如果都没找到，返回当前目录（保持原有行为）
    return currentPath;
}

QString MainWindow::findPL0File(const QString& filename) {
    // 首先尝试当前目录
    QString currentPath = QDir::currentPath();
    QString pl0File = currentPath + "/" + filename + ".PL0";
    if (QFile::exists(pl0File)) {
        return pl0File;
    }
    
    // 尝试test目录
    QString testDir = findTestDirectory();
    pl0File = testDir + "/" + filename + ".PL0";
    if (QFile::exists(pl0File)) {
        return pl0File;
    }
    
    // 如果都没找到，返回原始路径（让程序正常报错）
    return filename + ".PL0";
}
