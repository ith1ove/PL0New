#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstring>
class QTextEdit;
class QGroupBox;
class QRadioButton;
class QLabel;
class QLineEdit;
class QPushButton;
class QLayout;
class QTabBar;
class QButtonGroup;

//extern char *SYMOUT[];
typedef  int *SYMSET; // SET OF SYMBOL;
typedef  char ALFA[11];
typedef  enum { CONSTANT, VARIABLE, PROCEDUR, CHARCONST, CHARVARIABLE, REALCONST, REALVARIABLE } OBJECTS ;
typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC } FCT;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void printfs(const char* s);
    void printcs(const char c);
    void printls(const char *s0, long v);
    void printrs(const char *s0, float v);
    void pl0printf(QString s);

    void Error(int n);
    void GetCh();
    void GetSym();
    void GEN(FCT X, int Y, int Z);
    void TEST(SYMSET S1, SYMSET S2, int N);
    void ENTER(OBJECTS K, int LEV, int &TX, int &DX);
    int POSITION(ALFA ID, int TX);
    void ConstDeclaration(int LEV,int &TX,int &DX);
    void VarDeclaration(int LEV,int &TX,int &DX);
    void CharVarDeclaration(int LEV,int &TX,int &DX);  // 新增字符变量声明函数
    void RealVarDeclaration(int LEV,int &TX,int &DX);  // 新增实数变量声明函数
    void ListCode(int CX0);
    void FACTOR(SYMSET FSYS, int LEV, int &TX);
    void TERM(SYMSET FSYS, int LEV, int &TX);
    void EXPRESSION(SYMSET FSYS, int LEV, int &TX);
    void CONDITION(SYMSET FSYS,int LEV,int &TX);
    void STATEMENT(SYMSET FSYS,int LEV,int &TX);
    void Block(int LEV, int TX, SYMSET FSYS);
    int BASE(int L,int B,int S[]);
    void Interpret();

private:
    void createPL0Page();   // 定义创建 PL0编译器 页面函数
    void loadModernStyle(); // 加载现代化样式
    void applyBuiltinStyle(); // 应用内置样式

    QWidget *pl0Page;
    QTextEdit *pl0TextEdit; // 左文本框
    QLabel *sourceLabel;
    QLayout *rightLayout;
    QLineEdit *sourceLineEdit;
    QGroupBox *codeOptionsGroupBox;  // 重命名分组框
    QTabBar *codeVisibilityTabBar;   // 保留以兼容旧代码
    QButtonGroup *codeVisibilityButtonGroup; // 新增按钮组
    QPushButton *runButton;
    QPushButton *clearButton;  // 清空按钮

    // 个人班级、学号、姓名部分
    QGroupBox *idGroupBox;
    QLabel *classLabel;
    QLabel *noLabel;
    QLabel *nameLabel;

private slots:
    void runClicked();
    void clearOutput();
};

#endif // MAINWINDOW_H
