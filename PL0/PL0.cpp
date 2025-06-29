#include "mainwindow.h"
#include <QButtonGroup>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QTabBar>
#include <QTextEdit>
#include <cctype>
#include <cstring>
#include <iostream>

using namespace std;

//---------------------------------------------------------------------------
const int AL = 10; /* LENGTH OF IDENTIFIERS */
const int NORW = 28;
/* # OF RESERVED WORDS */ // 关键字个数（新增char, readc, writec, real, writereal, readreal, function）
const int TXMAX = 100;    /* LENGTH OF IDENTIFIER TABLE */
const int NMAX = 14;      /* MAX NUMBER OF DEGITS IN NUMBERS */
const int AMAX = 2047;    /* MAXIMUM ADDRESS */
const int LEVMAX = 3;     /* MAX DEPTH OF BLOCK NESTING */
const int CXMAX = 200;    /* SIZE OF CODE ARRAY */

const int SYMNUM = 64; // SYM个数（新增实数类型支持和函数支持）

typedef enum
{
    NUL,
    IDENT,
    NUMBER,
    PLUS,
    MINUS,
    TIMES,
    SLASH,
    ODDSYM,
    EQL,
    NEQ,
    LSS,
    LEQ,
    GTR,
    GEQ,
    LPAREN,
    RPAREN,
    COMMA,
    SEMICOLON,
    PERIOD,
    BECOMES,
    BEGINSYM,
    ENDSYM,
    IFSYM,
    THENSYM,
    WHILESYM,
    WRITESYM,
    READSYM,
    DOSYM,
    CALLSYM,
    CONSTSYM,
    VARSYM,
    PROCSYM,
    PROGSYM,
    FUNCTIONSYM // 新增函数关键字

    // ↓↓↓ 新增部分 ↓↓↓
    ,
    ELSESYM,
    FORSYM,
    STEPSYM,
    UNTILSYM,
    RETURNSYM, // 共5个。ELSE，FOR，STEP，UNTIL，RETURN
    TIMESBECOMES,
    SLASHBECOMES,
    ANDSYM,
    ORSYM,
    NOTSYM, // 共5个。*=，/=，&，||，！
    TOSYM,
    DOWNTOSYM,
    PLUSBECOMES,
    MINUSBECOMES,
    PLUSPLUS,
    MINUSMINUS, // 新增4个。+=，-=，++，--
    CHARSYM,
    CHARACTER, // 新增字符类型支持：'char'关键字和字符常量
    WRITECSYM,
    READCSYM, // 新增字符读写函数
    REALSYM,
    REALNUMBER, // 新增实数类型支持：'real'关键字和实数常量
    WRITEREALNUM,
    READREALNUM // 新增实数读写函数
                // ↑↑↑ 新增部分 ↑↑↑
} SYMBOL;
const char* SYMOUT[] = {
    "NUL",
    "IDENT",
    "NUMBER",
    "PLUS",
    "MINUS",
    "TIMES",
    "SLASH",
    "ODDSYM",
    "EQL",
    "NEQ",
    "LSS",
    "LEQ",
    "GTR",
    "GEQ",
    "LPAREN",
    "RPAREN",
    "COMMA",
    "SEMICOLON",
    "PERIOD",
    "BECOMES",
    "BEGINSYM",
    "ENDSYM",
    "IFSYM",
    "THENSYM",
    "WHILESYM",
    "WRITESYM",
    "READSYM",
    "DOSYM",
    "CALLSYM",
    "CONSTSYM",
    "VARSYM",
    "PROCSYM",
    "PROGSYM",
    "FUNCTIONSYM"

    // ↓↓↓ 新增部分 ↓↓↓
    ,
    "ELSESYM",
    "FORSYM",
    "STEPSYM",
    "UNTILSYM",
    "RETURNSYM", // 共5个。ELSE，FOR，STEP，UNTIL，RETURN
    "TIMESBECOMES",
    "SLASHBECOMES",
    "ANDSYM",
    "ORSYM",
    "NOTSYM", // 共5个。*=，/=，&，||，！
    "TOSYM",
    "DOWNTOSYM", // 新增2个。TO，DOWNTO
    "PLUSBECOMES",
    "MINUSBECOMES",
    "PLUSPLUS",
    "MINUSMINUS", // 新增4个。+=，-=，++，--
    "CHARSYM",
    "CHARACTER", // 新增字符类型支持
    "WRITECSYM",
    "READCSYM", // 新增字符读写函数
    "REALSYM",
    "REALNUMBER", // 新增实数类型支持
    "WRITEREALNUM",
    "READREALNUM" // 新增实数读写函数
    // ↑↑↑ 新增部分 ↑↑↑
};
// typedef  int *SYMSET; // SET OF SYMBOL;
// typedef  char ALFA[11];
// typedef  enum { CONSTANT, VARIABLE, PROCEDUR, REALCONST, CHARCONST, REALVARIABLE, CHARVARIABLE } OBJECTS;
// typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC } FCT;
typedef struct
{
    FCT F; /*FUNCTION CODE*/
    int L; /*0..LEVMAX  LEVEL*/
    int A; /*0..AMAX    DISPLACEMENT ADDR*/
} INSTRUCTION;
/* LIT O A -- LOAD CONSTANT A             */
/* OPR 0 A -- EXECUTE OPR A               */
/* LOD L A -- LOAD VARIABLE L,A           */
/* STO L A -- STORE VARIABLE L,A          */
/* CAL L A -- CALL PROCEDURE A AT LEVEL L */
/* INI 0 A -- INCREMET T-REGISTER BY A    */
/* JMP 0 A -- JUMP TO A                   */
/* JPC 0 A -- JUMP CONDITIONAL TO A       */
char CH;                                    /*LAST CHAR READ*/
SYMBOL SYM;                                 /*LAST SYMBOL READ*/
ALFA ID;                                    /*LAST IDENTIFIER READ*/
int NUM;                                    /*LAST NUMBER READ*/
double REALNUM; /*LAST REAL NUMBER READ*/   // 新增：存储实数值
char CHARVAL; /*LAST CHARACTER VALUE READ*/ // 新增：存储字符常量值
int CC;                                     /*CHARACTER COUNT*/
int LL;                                     /*LINE LENGTH*/
int CX;                                     /*CODE ALLOCATION INDEX*/
char LINE[81];
INSTRUCTION CODE[CXMAX];
ALFA KWORD[NORW + 1];
SYMBOL WSYM[NORW + 1];
SYMBOL SSYM['^' + 1];
ALFA MNEMONIC[10];
SYMSET DECLBEGSYS, STATBEGSYS, FACBEGSYS;

struct
{
    ALFA NAME;
    OBJECTS KIND;
    union
    {
        int VAL;                             /*CONSTANT*/
        double REALVAL; /*REAL CONSTANT*/    // 新增：实数常量值
        char CHARVAL; /*CHARACTER CONSTANT*/ // 新增：字符常量值
        struct
        {
            int LEVEL, ADR, SIZE;
            int PARAM_COUNT;         // 参数数量
            OBJECTS PARAM_TYPES[10]; // 参数类型数组
            OBJECTS RETURN_TYPE;     // 返回值类型（仅对FUNCTION有效）
        } vp;                        /*VARIABLE,PROCEDUR,FUNCTION:*/
    };
} TABLE[TXMAX];

FILE *FIN, *FOUT;
int ERR;

void EXPRESSION(SYMSET FSYS, int LEV, int& TX);
void TERM(SYMSET FSYS, int LEV, int& TX);
//---------------------------------------------------------------------------
int SymIn(SYMBOL SYM, SYMSET S1)
{
    return S1[SYM];
}
//---------------------------------------------------------------------------

SYMSET SymSetUnion(SYMSET S1, SYMSET S2)
{
    SYMSET S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (int i = 0; i < SYMNUM; i++)
        if (S1[i] || S2[i])
            S[i] = 1;
        else
            S[i] = 0;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetAdd(SYMBOL SY, SYMSET S)
{
    SYMSET S1;
    S1 = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (int i = 0; i < SYMNUM; i++)
        S1[i] = S[i];
    S1[SY] = 1;
    return S1;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    S[b] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    S[b] = 1;
    S[c] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    S[b] = 1;
    S[c] = 1;
    S[d] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d, SYMBOL e)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    S[b] = 1;
    S[c] = 1;
    S[d] = 1;
    S[e] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d, SYMBOL e, SYMBOL f)
{
    SYMSET S;
    int i, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    S[a] = 1;
    S[b] = 1;
    S[c] = 1;
    S[d] = 1;
    S[e] = 1;
    S[f] = 1;
    return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNULL()
{
    SYMSET S;
    int i, n, k;
    S = (SYMSET)malloc(sizeof(int) * SYMNUM);
    for (i = 0; i < SYMNUM; i++)
        S[i] = 0;
    return S;
}
//---------------------------------------------------------------------------
void MainWindow::Error(int n)
{
    QString qs = QString("***");
    for (int i = 0; i < CC - 1; i++)
    {
        qs.append(" ");
    }
    qs.append("^");
    string s = qs.toStdString();
    // BCB API:static AnsiString StringOfChar(char ch, int count)  返回一个包含count个字符ch的字符串。
    // string s = "***"+AnsiString::StringOfChar(' ', CC-1)+"^";
    //  Form1->printls(s.c_str(),n);   fprintf(FOUT,"%s%d\n", s.c_str(), n);
    printls(s.c_str(), n);
    fprintf(FOUT, "%s%d\n", s.c_str(), n);
    ERR++;
} /*Error*/
//---------------------------------------------------------------------------
void MainWindow::GetCh()
{
    if (CC == LL)
    {
        if (feof(FIN))
        {
            // 文件结束，安全返回
            CH = ' ';
            return;
        }
        LL = 0;
        CC = 0;
        CH = ' ';
        while (!feof(FIN) && CH != 10)
        {
            CH = fgetc(FIN);
            LINE[LL++] = CH;
        }
        LINE[LL - 1] = ' ';
        LINE[LL] = 0;
        string s = to_string(CX);
        while (s.length() < 3)
            s = " " + s;
        s = s + " " + LINE;
        printfs(s.c_str());
        fprintf(FOUT, "%s\n", s.c_str());
    }
    CH = LINE[CC++];
} /*GetCh()*/
//---------------------------------------------------------------------------
void MainWindow::GetSym()
{
    int i, J, K;
    ALFA A;
    while (CH <= ' ')
        GetCh();

    // 字符识别完成，移除调试输出

    // 处理注释 - 简化版本
    bool inComment = false;
    do
    {
        inComment = false;

        // 单行注释 (//)
        if (CH == '/' && CC < LL && LINE[CC] == '/')
        {
            // 跳过整行
            while (CC < LL)
                CC++;
            GetCh();
            inComment = true;
        }
        // 多行注释开始 (/*)
        else if (CH == '/' && CC < LL && LINE[CC] == '*')
        {
            CC++;
            GetCh(); // 跳过 /*

            // 简单处理多行注释
            while (!feof(FIN))
            {
                if (CH == '*' && CC < LL && LINE[CC] == '/')
                {
                    CC++;
                    GetCh(); // 跳过 */
                    break;
                }
                GetCh();
            }
            inComment = true;
        }

        // 注释后跳过空白
        if (inComment)
        {
            while (CH <= ' ')
                GetCh();
        }
    } while (inComment);
    if ((CH >= 'A' && CH <= 'Z') || (CH >= 'a' && CH <= 'z'))
    { /*ID OR RESERVED WORD*/
        K = 0;
        do
        {
            if (K < AL)
                A[K++] = toupper(CH); // 转换为大写存储
            GetCh();
        } while ((CH >= 'A' && CH <= 'Z') || (CH >= 'a' && CH <= 'z') || (CH >= '0' && CH <= '9'));
        A[K] = '\0';
        strcpy(ID, A);
        i = 1;
        J = NORW;
        int found = 0;
        while (i <= J)
        {
            K = (i + J) / 2;
            int cmp = strcmp(ID, KWORD[K]);
            if (cmp == 0)
            {
                SYM = WSYM[K];
                found = 1;
                break;
            }
            else if (cmp < 0)
            {
                J = K - 1;
            }
            else
            {
                i = K + 1;
            }
        }
        if (!found)
        {
            SYM = IDENT;
        }
    }
    else if (CH >= '0' && CH <= '9')
    { /*NUMBER or REAL NUMBER*/
        K = 0;
        NUM = 0;
        REALNUM = 0.0;
        SYM = NUMBER;
        bool isReal = false;

        // 读取整数部分
        do
        {
            NUM = 10 * NUM + (CH - '0');
            REALNUM = REALNUM * 10.0 + (CH - '0');
            K++;
            GetCh();
        } while (CH >= '0' && CH <= '9');

        // 检查是否有小数点
        if (CH == '.')
        {
            isReal = true;
            SYM = REALNUMBER;
            GetCh();

            // 读取小数部分
            double fraction = 0.1;
            while (CH >= '0' && CH <= '9')
            {
                REALNUM += (CH - '0') * fraction;
                fraction /= 10.0;
                K++;
                GetCh();
            }
        }

        if (K > NMAX)
            Error(30);
    }
    else if (CH == '\'')
    { /*CHARACTER CONSTANT*/
        GetCh();
        if (CH == '\'')
        {
            Error(50); // 空字符常量
        }
        else if (CH == '\\')
        {
            // 处理转义字符
            GetCh();
            switch (CH)
            {
            case 'n':
                CHARVAL = '\n';
                break;
            case 't':
                CHARVAL = '\t';
                break;
            case 'r':
                CHARVAL = '\r';
                break;
            case '\\':
                CHARVAL = '\\';
                break;
            case '\'':
                CHARVAL = '\'';
                break;
            case '0':
                CHARVAL = '\0';
                break;
            default:
                CHARVAL = CH; // 其他转义字符直接使用
                break;
            }
            GetCh();
            if (CH != '\'')
            {
                Error(51); // 缺少右单引号
            }
            else
            {
                SYM = CHARACTER;
                GetCh();
            }
        }
        else
        {
            // 普通字符
            CHARVAL = CH;
            GetCh();
            if (CH != '\'')
            {
                Error(51); // 缺少右单引号
            }
            else
            {
                SYM = CHARACTER;
                GetCh();
            }
        }
    }
    else if (CH == ':')
    {
        GetCh();
        if (CH == '=')
        {
            SYM = BECOMES;
            GetCh();
        }
        else
            SYM = NUL;
    }
    else /* THE FOLLOWING TWO CHECK WERE ADDED
     BECAUSE ASCII DOES NOT HAVE A SINGLE CHARACTER FOR <= OR >= */
        if (CH == '<')
        {
            GetCh();
            if (CH == '=')
            {
                SYM = LEQ;
                GetCh();
            }
            else
                SYM = LSS;
        }
        else if (CH == '>')
        {
            GetCh();
            if (CH == '=')
            {
                SYM = GEQ;
                GetCh();
            }
            else
                SYM = GTR;
        }
        else if (CH == '*')
        { // 运算符 *=
            GetCh();
            if (CH == '=')
            {
                SYM = TIMESBECOMES;
                GetCh();
            }
            else
                SYM = TIMES;
        }
        else if (CH == '/')
        { // 运算符 /=
            GetCh();
            if (CH == '=')
            {
                SYM = SLASHBECOMES;
                GetCh();
            }
            else
                SYM = SLASH;
        }
        else if (CH == '+')
        { // 运算符 +=, ++
            GetCh();
            if (CH == '=')
            {
                SYM = PLUSBECOMES;
                GetCh();
            }
            else if (CH == '+')
            {
                SYM = PLUSPLUS;
                GetCh();
            }
            else
                SYM = PLUS;
        }
        else if (CH == '-')
        { // 运算符 -=, --
            GetCh();
            if (CH == '=')
            {
                SYM = MINUSBECOMES;
                GetCh();
            }
            else if (CH == '-')
            {
                SYM = MINUSMINUS;
                GetCh();
            }
            else
                SYM = MINUS;
        }
        else if (CH == '!')
        { // 运算符 !=，单独的 ! 为非法
            GetCh();
            if (CH == '=')
            {
                SYM = NEQ;
                GetCh();
            }
            else
            {
                Error(19); // 单独的 ! 报错
            }
        }
        else if (CH == '|')
        { // 运算符 ||
            GetCh();
            if (CH == '|')
            {
                SYM = ORSYM;
                GetCh();
            }
            else
            {
                Error(19); // 单独的 | 报错
            }
        }
        else
        {
            SYM = SSYM[(unsigned char)CH];
            GetCh();
        }
} /*GetSym()*/
//---------------------------------------------------------------------------
void MainWindow::GEN(FCT X, int Y, int Z)
{
    if (CX > CXMAX)
    {
        //    Form1->printfs("PROGRAM TOO LONG");
        printfs("PROGRAM TOO LONG");
        fprintf(FOUT, "PROGRAM TOO LONG\n");
        fclose(FOUT);
        exit(0);
    }
    CODE[CX].F = X;
    CODE[CX].L = Y;
    CODE[CX].A = Z;
    CX++;
} /*GEN*/
//---------------------------------------------------------------------------
void MainWindow::TEST(SYMSET S1, SYMSET S2, int N)
{
    if (!SymIn(SYM, S1))
    {
        Error(N);
        while (!SymIn(SYM, SymSetUnion(S1, S2)))
            GetSym();
    }
} /*TEST*/
//---------------------------------------------------------------------------
void MainWindow::ENTER(OBJECTS K, int LEV, int& TX, int& DX)
{ /*ENTER OBJECT INTO TABLE*/
    TX++;
    strcpy(TABLE[TX].NAME, ID);
    TABLE[TX].KIND = K;

    // 初始化union成员
    memset(&TABLE[TX].vp, 0, sizeof(TABLE[TX].vp));

    // 调试输出（简化）
    // char debug_buf[200];
    // sprintf(debug_buf, "ENTER: %s KIND=%d LEV=%d TX=%d DX=%d", ID, K, LEV, TX, DX);
    // printfs(debug_buf);

    switch (K)
    {
    case CONSTANT:
        if (NUM > AMAX)
        {
            Error(31);
            NUM = 0;
        }
        TABLE[TX].VAL = NUM;
        break;
    case REALCONST: // 新增实数常量处理
        TABLE[TX].REALVAL = REALNUM;
        break;
    case CHARCONST: // 新增字符常量处理
        TABLE[TX].CHARVAL = CHARVAL;
        break;
    case VARIABLE:
        TABLE[TX].vp.LEVEL = LEV;
        TABLE[TX].vp.ADR = DX;
        DX++;
        // sprintf(debug_buf, "VARIABLE: %s NEW addr=%d", ID, TABLE[TX].vp.ADR);
        // printfs(debug_buf);
        break;
    case PARAMETER: // 新增参数处理
        TABLE[TX].vp.LEVEL = LEV;
        // 参数地址不在这里设置，而是在Block函数中设置为负数
        // 这里只是占位，实际地址会在Block函数中被覆盖
        // sprintf(debug_buf, "PARAMETER: %s addr=%d (will be set in Block)", ID, TABLE[TX].vp.ADR);
        // printfs(debug_buf);
        break;
    case REALVARIABLE: // 新增实数变量处理
        TABLE[TX].vp.LEVEL = LEV;
        TABLE[TX].vp.ADR = DX;
        DX++;
        break;
    case CHARVARIABLE: // 新增字符变量处理
        TABLE[TX].vp.LEVEL = LEV;
        TABLE[TX].vp.ADR = DX;
        DX++;
        break;
    case PROCEDUR:
        TABLE[TX].vp.LEVEL = LEV;
        TABLE[TX].vp.PARAM_COUNT = 0;        // 初始化参数数量
        TABLE[TX].vp.RETURN_TYPE = VARIABLE; // 默认返回类型
        break;
    case FUNCTION: // 新增函数处理
        TABLE[TX].vp.LEVEL = LEV;
        TABLE[TX].vp.PARAM_COUNT = 0;        // 初始化参数数量
        TABLE[TX].vp.RETURN_TYPE = VARIABLE; // 默认返回类型，会在函数定义时更新
        break;
    }
} /*ENTER*/
//---------------------------------------------------------------------------
int MainWindow::POSITION(ALFA ID, int TX)
{ /*FIND IDENTIFIER IN TABLE*/
    int i = TX;
    strcpy(TABLE[0].NAME, ID);
    while (strcmp(TABLE[i].NAME, ID) != 0)
        i--;

    // 调试输出（简化）
    // char debug_buf[200];
    // sprintf(debug_buf, "POSITION: %s found at %d, KIND=%d, ADR=%d", ID, i, TABLE[i].KIND, TABLE[i].vp.ADR);
    // printfs(debug_buf);

    // 如果是参数类型，额外检查
    // if (TABLE[i].KIND == PARAMETER) {
    //     sprintf(debug_buf, "PARAMETER CHECK: %s TX=%d, LEVEL=%d, ADR=%d", ID, i, TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
    //     printfs(debug_buf);
    // }

    return i;
} /*POSITION*/
//---------------------------------------------------------------------------
void MainWindow::ConstDeclaration(int LEV, int& TX, int& DX)
{
    if (SYM == IDENT)
    {
        GetSym();
        if (SYM == EQL || SYM == BECOMES)
        {
            if (SYM == BECOMES)
                Error(1);
            GetSym();
            if (SYM == NUMBER)
            {
                ENTER(CONSTANT, LEV, TX, DX);
                GetSym();
            }
            else if (SYM == REALNUMBER)
            {
                ENTER(REALCONST, LEV, TX, DX);
                GetSym();
            }
            else if (SYM == CHARACTER)
            {
                ENTER(CHARCONST, LEV, TX, DX);
                GetSym();
            }
            else
                Error(2);
        }
        else
            Error(3);
    }
    else
        Error(4);
} /*ConstDeclaration()*/
//---------------------------------------------------------------------------
void MainWindow::VarDeclaration(int LEV, int& TX, int& DX)
{
    if (SYM == IDENT)
    {
        ENTER(VARIABLE, LEV, TX, DX);
        GetSym();
    }
    else
        Error(4);
} /*VarDeclaration()*/

void MainWindow::CharVarDeclaration(int LEV, int& TX, int& DX)
{
    if (SYM == IDENT)
    {
        ENTER(CHARVARIABLE, LEV, TX, DX);
        GetSym();
    }
    else
        Error(4);
} /*CharVarDeclaration()*/

void MainWindow::RealVarDeclaration(int LEV, int& TX, int& DX)
{
    if (SYM == IDENT)
    {
        ENTER(REALVARIABLE, LEV, TX, DX);
        GetSym();
    }
    else
        Error(4);
} /*RealVarDeclaration()*/
//---------------------------------------------------------------------------
void MainWindow::ListCode(int CX0)
{ /*LIST CODE GENERATED FOR THIS Block*/
    // 使用按钮组判断：0表示"显示"，1表示"不显示"
    bool shouldShowCode = (codeVisibilityButtonGroup != nullptr && codeVisibilityButtonGroup->checkedId() == 0);
    if (shouldShowCode)
        for (int i = CX0; i < CX; i++)
        {
            string s = to_string(i);
            while (s.length() < 3)
                s = " " + s;
            s = s + " " + MNEMONIC[CODE[i].F] + " " + to_string(CODE[i].L) + " " + to_string(CODE[i].A);
            //      Form1->printfs(s.c_str());
            printfs(s.c_str());
            //      fprintf(FOUT,"%3d%5s%4d%4d\n",i,MNEMONIC[CODE[i].F],CODE[i].L,CODE[i].A);
            fprintf(FOUT, "%s\n", s.c_str());
        }
} /*ListCode()*/
//---------------------------------------------------------------------------
void MainWindow::FACTOR(SYMSET FSYS, int LEV, int& TX)
{
    int i;
    TEST(FACBEGSYS, FSYS, 24);
    while (SymIn(SYM, FACBEGSYS))
    {

        // 处理前缀 ++/-- 运算符
        if (SYM == PLUSPLUS || SYM == MINUSMINUS)
        {
            SYMBOL preOp = SYM;
            GetSym();
            if (SYM == IDENT)
            {
                i = POSITION(ID, TX);
                if (i == 0)
                    Error(11);
                else if (TABLE[i].KIND != VARIABLE && TABLE[i].KIND != CHARVARIABLE)
                    Error(12);
                else
                {
                    // 前缀++: 先增加变量值，然后返回新值
                    // 加载变量当前值
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    GEN(LIT, 0, 1); // 压入常数1
                    if (preOp == PLUSPLUS)
                        GEN(OPR, 0, 2); // 加法
                    else
                        GEN(OPR, 0, 3); // 减法
                    // 存储新值
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    // 再次加载新值到栈顶（作为表达式的值）
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                }
                GetSym();
            }
            else
                Error(4); // 期望变量名
        }
        else if (SYM == IDENT)
        {
            i = POSITION(ID, TX);
            if (i == 0)
                Error(11);
            else
                switch (TABLE[i].KIND)
                {
                case CONSTANT:
                    GEN(LIT, 0, TABLE[i].VAL);
                    break;
                case REALCONST: // 新增实数常量处理
                    // 需要特殊的指令处理实数，暂时转换为整数处理
                    GEN(LIT, 0, (int)(TABLE[i].REALVAL * 1000)); // 放大1000倍存储
                    break;
                case CHARCONST:                         // 新增字符常量处理
                    GEN(LIT, 0, (int)TABLE[i].CHARVAL); // 将字符转换为整数
                    break;
                case VARIABLE:
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    break;
                case PARAMETER: // 新增参数处理
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    break;
                case REALVARIABLE: // 新增实数变量处理
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    break;
                case CHARVARIABLE: // 新增字符变量处理
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    break;
                case PROCEDUR:
                    Error(21);
                    break;
                case FUNCTION: // 新增函数调用处理
                    // 解析函数参数
                    GetSym();
                    if (SYM == LPAREN)
                    {
                        GetSym();
                        int paramCount = 0;
                        if (SYM != RPAREN)
                        {
                            EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                            paramCount++;
                            while (SYM == COMMA)
                            {
                                GetSym();
                                EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                                paramCount++;
                            }
                        }
                        if (SYM == RPAREN)
                        {
                            GetSym();
                        }
                        else
                        {
                            Error(22); // 缺少右括号
                        }
                        // 检查参数数量
                        if (paramCount != TABLE[i].vp.PARAM_COUNT)
                        {
                            Error(39); // 参数数量不匹配
                        }
                        // 生成函数调用指令
                        GEN(CAL, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    }
                    else
                    {
                        // 无参数函数调用
                        if (TABLE[i].vp.PARAM_COUNT != 0)
                        {
                            Error(39); // 参数数量不匹配
                        }
                        GEN(CAL, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    }
                    return; // 函数调用不需要再GetSym()
                    break;
                }
            GetSym();

            // 检查后缀 ++/-- 运算符
            if (SYM == PLUSPLUS || SYM == MINUSMINUS)
            {
                if (TABLE[i].KIND != VARIABLE && TABLE[i].KIND != CHARVARIABLE && TABLE[i].KIND != REALVARIABLE)
                {
                    Error(12); // 不是变量
                }
                else
                {
                    SYMBOL postOp = SYM;
                    // 后缀++: 先返回旧值，然后增加变量值
                    // 当前栈顶已有变量的旧值
                    // 再加载一次变量值用于计算
                    GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    GEN(LIT, 0, 1); // 压入常数1
                    if (postOp == PLUSPLUS)
                        GEN(OPR, 0, 2); // 加法
                    else
                        GEN(OPR, 0, 3); // 减法
                    // 存储新值
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                    // 栈顶仍然是旧值（作为表达式的值）
                }
                GetSym();
            }
        }
        else if (SYM == NUMBER)
        {
            if (NUM > AMAX)
            {
                Error(31);
                NUM = 0;
            }
            GEN(LIT, 0, NUM);
            GetSym();
        }
        else if (SYM == REALNUMBER)
        { // 新增实数常量处理
            // 将实数转换为整数处理（放大1000倍）
            int scaledReal = (int)(REALNUM * 1000);
            GEN(LIT, 0, scaledReal);
            GetSym();
        }
        else if (SYM == CHARACTER)
        {                              // 新增字符常量处理
            GEN(LIT, 0, (int)CHARVAL); // 将字符转换为整数压栈
            GetSym();
        }
        else if (SYM == LPAREN)
        {
            GetSym();
            EXPRESSION(SymSetAdd(RPAREN, FSYS), LEV, TX);
            if (SYM == RPAREN)
                GetSym();
            else
                Error(22);
        }
        TEST(FSYS, FACBEGSYS, 23);
    }
} /*FACTOR*/
//---------------------------------------------------------------------------
void MainWindow::TERM(SYMSET FSYS, int LEV, int& TX)
{ /*TERM*/
    SYMBOL MULOP;
    FACTOR(SymSetUnion(FSYS, SymSetNew(TIMES, SLASH)), LEV, TX);
    while (SYM == TIMES || SYM == SLASH)
    {
        MULOP = SYM;
        GetSym();
        FACTOR(SymSetUnion(FSYS, SymSetNew(TIMES, SLASH)), LEV, TX);
        if (MULOP == TIMES)
            GEN(OPR, 0, 4);
        else
            GEN(OPR, 0, 5);
    }
} /*TERM*/
//---------------------------------------------------------------------------
void MainWindow::EXPRESSION(SYMSET FSYS, int LEV, int& TX)
{
    SYMBOL ADDOP;
    if (SYM == PLUS || SYM == MINUS)
    {
        ADDOP = SYM;
        GetSym();
        TERM(SymSetUnion(FSYS, SymSetNew(PLUS, MINUS)), LEV, TX);
        if (ADDOP == MINUS)
            GEN(OPR, 0, 1);
    }
    else
        TERM(SymSetUnion(FSYS, SymSetNew(PLUS, MINUS)), LEV, TX);
    while (SYM == PLUS || SYM == MINUS)
    {
        ADDOP = SYM;
        GetSym();
        TERM(SymSetUnion(FSYS, SymSetNew(PLUS, MINUS)), LEV, TX);
        if (ADDOP == PLUS)
            GEN(OPR, 0, 2);
        else
            GEN(OPR, 0, 3);
    }
} /*EXPRESSION*/
//---------------------------------------------------------------------------
void MainWindow::CONDITION(SYMSET FSYS, int LEV, int& TX)
{
    SYMBOL RELOP;
    if (SYM == ODDSYM)
    {
        GetSym();
        EXPRESSION(FSYS, LEV, TX);
        GEN(OPR, 0, 6);
    }
    else
    {
        EXPRESSION(SymSetUnion(SymSetNew(EQL, NEQ, LSS, LEQ, GTR, GEQ), FSYS), LEV, TX);
        if (!SymIn(SYM, SymSetNew(EQL, NEQ, LSS, LEQ, GTR, GEQ)))
            Error(20);
        else
        {
            RELOP = SYM;
            GetSym();
            EXPRESSION(FSYS, LEV, TX);
            switch (RELOP)
            {
            case EQL:
                GEN(OPR, 0, 8);
                break;
            case NEQ:
                GEN(OPR, 0, 9);
                break;
            case LSS:
                GEN(OPR, 0, 10);
                break;
            case GEQ:
                GEN(OPR, 0, 11);
                break;
            case GTR:
                GEN(OPR, 0, 12);
                break;
            case LEQ:
                GEN(OPR, 0, 13);
                break;
            }
        }
    }
} /*CONDITION*/
//---------------------------------------------------------------------------
void MainWindow::STATEMENT(SYMSET FSYS, int LEV, int& TX)
{ /*STATEMENT*/
    int i, CX1, CX2, CX3;

    switch (SYM)
    {
    case IDENT:
        i = POSITION(ID, TX);
        if (i == 0)
            Error(11);
        else if (TABLE[i].KIND != VARIABLE && TABLE[i].KIND != CHARVARIABLE && TABLE[i].KIND != REALVARIABLE)
        { /*ASSIGNMENT TO NON-VARIABLE*/
            Error(12);
            i = 0;
        }
        GetSym();

        if (SYM == BECOMES)
        {
            GetSym();
            EXPRESSION(FSYS, LEV, TX);
            if (i != 0)
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
        }
        else if (SYM == PLUSBECOMES)
        {
            // += 运算符的实现：var += expr 等价于 var := var + expr
            GetSym();
            if (i != 0)
            {
                // 先加载变量当前值到栈顶
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                // 解析右侧表达式
                EXPRESSION(FSYS, LEV, TX);
                // 执行加法运算
                GEN(OPR, 0, 2);                                     // 加法运算
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR); // 存储结果
            }
            else
            {
                // 如果变量无效，仍需要解析右侧表达式以保持语法一致性
                EXPRESSION(FSYS, LEV, TX);
            }
        }
        else if (SYM == MINUSBECOMES)
        {
            // -= 运算符的实现：var -= expr 等价于 var := var - expr
            GetSym();
            if (i != 0)
            {
                // 先加载变量当前值到栈顶
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                // 解析右侧表达式
                EXPRESSION(FSYS, LEV, TX);
                // 执行减法运算
                GEN(OPR, 0, 3);                                     // 减法运算
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR); // 存储结果
            }
            else
            {
                // 如果变量无效，仍需要解析右侧表达式以保持语法一致性
                EXPRESSION(FSYS, LEV, TX);
            }
        }
        else if (SYM == PLUSPLUS)
        {
            // 后缀++语句：var++;
            GetSym();
            if (i != 0)
            {
                // 后缀++: 加载变量，加1，存储（不需要返回值）
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                GEN(LIT, 0, 1);
                GEN(OPR, 0, 2); // 加法
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
            }
        }
        else if (SYM == MINUSMINUS)
        {
            // 后缀--语句：var--;
            GetSym();
            if (i != 0)
            {
                // 后缀--: 加载变量，减1，存储（不需要返回值）
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                GEN(LIT, 0, 1);
                GEN(OPR, 0, 3); // 减法
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
            }
        }
        else
            Error(13);
        break;
    case READSYM:
        GetSym();
        if (SYM != LPAREN)
            Error(34);
        else
            do
            {
                GetSym();
                if (SYM == IDENT)
                    i = POSITION(ID, TX);
                else
                    i = 0;
                if (i == 0)
                    Error(35);
                else
                {
                    // 根据变量类型选择不同的读取操作
                    if (TABLE[i].KIND == CHARVARIABLE)
                    {
                        GEN(OPR, 0, 21); // 字符输入
                    }
                    else
                    {
                        GEN(OPR, 0, 16); // 整数输入
                    }
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                }
                GetSym();
            } while (SYM == COMMA);
        if (SYM != RPAREN)
        {
            Error(33);
            while (!SymIn(SYM, FSYS))
                GetSym();
        }
        else
            GetSym();
        break; /* READSYM */
    case WRITESYM:
        GetSym();
        if (SYM == LPAREN)
        {
            do
            {
                GetSym();
                EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                GEN(OPR, 0, 14);
            } while (SYM == COMMA);
            if (SYM != RPAREN)
                Error(33);
            else
                GetSym();
        }
        GEN(OPR, 0, 15);
        break;      /*WRITESYM*/
    case WRITECSYM: // 新增字符输出处理
        GetSym();
        if (SYM == LPAREN)
        {
            do
            {
                GetSym();
                EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                GEN(OPR, 0, 20); // 使用字符输出操作码
            } while (SYM == COMMA);
            if (SYM != RPAREN)
                Error(33);
            else
                GetSym();
        }
        GEN(OPR, 0, 15);
        break;     /*WRITECSYM*/
    case READCSYM: // 新增字符输入处理
        GetSym();
        if (SYM != LPAREN)
            Error(34);
        else
            do
            {
                GetSym();
                if (SYM == IDENT)
                    i = POSITION(ID, TX);
                else
                    i = 0;
                if (i == 0)
                    Error(35);
                else
                {
                    GEN(OPR, 0, 21); // 字符输入
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                }
                GetSym();
            } while (SYM == COMMA);
        if (SYM != RPAREN)
        {
            Error(33);
            while (!SymIn(SYM, FSYS))
                GetSym();
        }
        else
            GetSym();
        break;         /* READCSYM */
    case WRITEREALNUM: // 新增实数输出处理
        GetSym();
        if (SYM == LPAREN)
        {
            do
            {
                GetSym();
                EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                GEN(OPR, 0, 22); // 使用实数输出操作码
            } while (SYM == COMMA);
            if (SYM != RPAREN)
                Error(33);
            else
                GetSym();
        }
        GEN(OPR, 0, 15);
        break;        /*WRITEREALNUM*/
    case READREALNUM: // 新增实数输入处理
        GetSym();
        if (SYM != LPAREN)
            Error(34);
        else
            do
            {
                GetSym();
                if (SYM == IDENT)
                    i = POSITION(ID, TX);
                else
                    i = 0;
                if (i == 0)
                    Error(35);
                else
                {
                    GEN(OPR, 0, 23); // 实数输入
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                }
                GetSym();
            } while (SYM == COMMA);
        if (SYM != RPAREN)
        {
            Error(33);
            while (!SymIn(SYM, FSYS))
                GetSym();
        }
        else
            GetSym();
        break; /* READREALNUM */
    case CALLSYM:
        GetSym();
        if (SYM != IDENT)
            Error(14);
        else
        {
            i = POSITION(ID, TX);
            if (i == 0)
                Error(11);
            else
            {
                if (TABLE[i].KIND == PROCEDUR || TABLE[i].KIND == FUNCTION)
                {
                    GetSym();
                    // 检查是否有参数列表
                    if (SYM == LPAREN)
                    {
                        GetSym();
                        int paramCount = 0;
                        if (SYM != RPAREN)
                        {
                            EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                            paramCount++;
                            while (SYM == COMMA)
                            {
                                GetSym();
                                EXPRESSION(SymSetUnion(SymSetNew(RPAREN, COMMA), FSYS), LEV, TX);
                                paramCount++;
                            }
                        }
                        if (SYM == RPAREN)
                        {
                            GetSym();
                        }
                        else
                        {
                            Error(22); // 缺少右括号
                        }
                        // 检查参数数量
                        if (paramCount != TABLE[i].vp.PARAM_COUNT)
                        {
                            Error(39); // 参数数量不匹配
                        }
                    }
                    else
                    {
                        // 无参数调用
                        if (TABLE[i].vp.PARAM_COUNT != 0)
                        {
                            Error(39); // 参数数量不匹配
                        }
                    }
                    GEN(CAL, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                }
                else
                {
                    Error(15);
                }
            }
        }
        break;
    case IFSYM:
        GetSym();
        CONDITION(SymSetUnion(SymSetNew(THENSYM, DOSYM), FSYS), LEV, TX);
        if (SYM == THENSYM)
            GetSym();
        else
            Error(16);
        CX1 = CX;
        GEN(JPC, 0, 0);
        // ↓↓↓ 新增部分 ↓↓↓
        STATEMENT(SymSetUnion(SymSetNew(ELSESYM), FSYS), LEV, TX);
        if (SYM != ELSESYM)
            CODE[CX1].A = CX;
        else
        {
            GetSym();
            CX2 = CX;
            GEN(JMP, 0, 0);   // 直接跳转，执行完Then后面的则跳转到条件语句最后面
            CODE[CX1].A = CX; // 回填条件跳转，填回else语句块中第一句
            STATEMENT(FSYS, LEV, TX);
            CODE[CX2].A = CX; // 回填直接跳转地址
        }
        // ↑↑↑ 新增部分 ↑↑↑
        // STATEMENT(FSYS,LEV,TX);  CODE[CX1].A=CX;
        break;
    case BEGINSYM:
        GetSym();
        STATEMENT(SymSetUnion(SymSetNew(SEMICOLON, ENDSYM), FSYS), LEV, TX);
        while (SymIn(SYM, SymSetAdd(SEMICOLON, STATBEGSYS)))
        {
            if (SYM == SEMICOLON)
                GetSym();
            else
                Error(10);
            STATEMENT(SymSetUnion(SymSetNew(SEMICOLON, ENDSYM), FSYS), LEV, TX);
        }
        if (SYM == ENDSYM)
            GetSym();
        else
            Error(17);
        break;
    case WHILESYM:
        CX1 = CX;
        GetSym();
        CONDITION(SymSetAdd(DOSYM, FSYS), LEV, TX);
        CX2 = CX;
        GEN(JPC, 0, 0);
        if (SYM == DOSYM)
            GetSym();
        else
            Error(18);
        STATEMENT(FSYS, LEV, TX);
        GEN(JMP, 0, CX1);
        CODE[CX2].A = CX;
        break;
        // ↓↓↓ FOR语句的简化实现 ↓↓↓
    case FORSYM:
        GetSym();
        if (SYM != IDENT)
        {
            Error(14); // 期望变量名
        }
        else
        {
            // 获取循环变量
            i = POSITION(ID, TX);
            if (i == 0)
            {
                Error(11); // 变量未声明
            }
            else if (TABLE[i].KIND != VARIABLE)
            {
                Error(12); // 不是变量
                i = 0;
            }
            GetSym();

            if (SYM != BECOMES)
            {
                Error(13); // 期望 :=
            }
            else
            {
                GetSym();
                // 解析并设置初始值
                EXPRESSION(SymSetUnion(SymSetNew(TOSYM, DOWNTOSYM), FSYS), LEV, TX);
                if (i != 0)
                {
                    GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR); // 存储初始值
                }

                // 检查是TO还是DOWNTO
                bool isDownTo = false;
                if (SYM == TOSYM)
                {
                    GetSym();
                }
                else if (SYM == DOWNTOSYM)
                {
                    GetSym();
                    isDownTo = true;
                }
                else
                {
                    Error(40); // 期望TO或DOWNTO
                }

                // 为了简化实现，我们将终止值计算并存储到下一个可用的变量位置
                // 进入一个新的表项来临时存储终止值
                TX++; // 分配临时变量
                TABLE[TX].KIND = VARIABLE;
                TABLE[TX].vp.LEVEL = LEV;
                TABLE[TX].vp.ADR = 999; // 使用一个固定地址作为临时存储
                strcpy(TABLE[TX].NAME, "__TEMP__");

                // 解析终止值表达式并存储
                EXPRESSION(SymSetAdd(DOSYM, FSYS), LEV, TX);
                GEN(STO, 0, 999); // 存储到临时位置

                if (SYM != DOSYM)
                {
                    Error(18); // 期望DO
                }
                else
                {
                    GetSym();

                    // 循环开始位置
                    CX1 = CX;

                    // 生成循环条件检查
                    if (i != 0)
                    {
                        // 加载循环变量
                        GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                        // 加载终止值进行比较
                        GEN(LOD, 0, 999);

                        // 生成条件判断 - 修正逻辑：当条件为假时才退出循环
                        if (isDownTo)
                        {
                            // FOR i := start DOWNTO end: 当 i >= end 时继续，i < end 时退出
                            GEN(OPR, 0, 11); // 大于等于比较: i >= end
                        }
                        else
                        {
                            // FOR i := start TO end: 当 i <= end 时继续，i > end 时退出
                            GEN(OPR, 0, 13); // 小于等于比较: i <= end
                        }

                        CX2 = CX;
                        GEN(JPC, 0, 0); // 如果比较结果为false(0)则跳出循环

                        // 执行循环体
                        STATEMENT(FSYS, LEV, TX);

                        // 更新循环变量
                        GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR); // 加载循环变量
                        GEN(LIT, 0, 1);                                     // 加载常数1
                        if (isDownTo)
                        {
                            GEN(OPR, 0, 3); // 减法: i = i - 1
                        }
                        else
                        {
                            GEN(OPR, 0, 2); // 加法: i = i + 1
                        }
                        GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR); // 存储更新后的值

                        // 跳回循环开始
                        GEN(JMP, 0, CX1);

                        // 回填跳出循环的地址
                        CODE[CX2].A = CX;
                    }
                    else
                    {
                        // 变量无效的情况，仍需要执行语句以保持语法一致性
                        STATEMENT(FSYS, LEV, TX);
                    }
                }

                // 恢复符号表状态
                TX--; // 移除临时变量
            }
        }
        break;
    case STEPSYM:
        GetSym();
        printfs("保留字：STEPSYM~~~~");
        break;
    case UNTILSYM:
        GetSym();
        printfs("保留字：UNTILSYM~~~~");
        break;
    case RETURNSYM: // 新增返回语句处理
        GetSym();
        if (LEV == 0)
        {
            Error(40); // 不能在主程序中使用RETURN
        }
        else
        {
            // 检查是否有返回值表达式
            if (!SymIn(SYM, SymSetNew(SEMICOLON, ENDSYM)))
            {
                // 有返回值
                EXPRESSION(SymSetAdd(SEMICOLON, FSYS), LEV, TX);
                GEN(RET, 0, 1); // 带返回值的返回指令
            }
            else
            {
                // 无返回值（过程返回）
                GEN(OPR, 0, 0); // 标准的过程返回
            }
        }
        break;
    case DOSYM:
        GetSym();
        printfs("保留字：DOSYM~~~~");
        break;

        // 用来检验运算符是否添加成功的标志。
    case TIMESBECOMES:
        GetSym();
        printfs("运算符：*= ~~~~");
        break;
    case SLASHBECOMES:
        GetSym();
        printfs("运算符：/= ~~~~");
        break;
    case PLUSPLUS:
        // 前缀++语句：++var;
        GetSym();
        if (SYM == IDENT)
        {
            i = POSITION(ID, TX);
            if (i == 0)
                Error(11);
            else if (TABLE[i].KIND != VARIABLE)
                Error(12);
            else
            {
                // 前缀++: 加载变量，加1，存储
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                GEN(LIT, 0, 1);
                GEN(OPR, 0, 2); // 加法
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
            }
            GetSym();
        }
        else
            Error(4); // 期望变量名
        break;
    case MINUSMINUS:
        // 前缀--语句：--var;
        GetSym();
        if (SYM == IDENT)
        {
            i = POSITION(ID, TX);
            if (i == 0)
                Error(11);
            else if (TABLE[i].KIND != VARIABLE)
                Error(12);
            else
            {
                // 前缀--: 加载变量，减1，存储
                GEN(LOD, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
                GEN(LIT, 0, 1);
                GEN(OPR, 0, 3); // 减法
                GEN(STO, LEV - TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);
            }
            GetSym();
        }
        else
            Error(4); // 期望变量名
        break;
    case ANDSYM:
        GetSym();
        printfs("运算符：&  ~~~~");
        break;
    case ORSYM:
        GetSym();
        printfs("运算符：|| ~~~~");
        break;
    case TOSYM:
        GetSym();
        printfs("保留字：TOSYM~~~~");
        break;
    case DOWNTOSYM:
        GetSym();
        printfs("保留字：DOWNTOSYM~~~~");
        break;
        // ↑↑↑ 新增部分 ↑↑↑
    }

    TEST(FSYS, SymSetNULL(), 19);
} /*STATEMENT*/
//---------------------------------------------------------------------------
void MainWindow::Block(int LEV, int TX, SYMSET FSYS)
{
    int DX = 3;          /*DATA ALLOCATION INDEX*/
    int TX0 = TX;        /*INITIAL TABLE INDEX*/
    int CX0 = CX;        /*INITIAL CODE INDEX*/
    // char debug_buf[200]; // 统一声明调试缓冲区 (removed)

    // 只在主程序（LEV=0）时设置地址和生成跳转指令
    if (LEV == 0)
    {
        TABLE[TX].vp.ADR = CX;
        GEN(JMP, 0, 0);
    }
    if (LEV > LEVMAX)
        Error(32);
    do
    {
        // 在每个声明处理前检查参数地址
        /*
        if (LEV > 0) {
            // Debug: BLOCK LOOP check (removed)
            for (int i = 1; i <= TX; i++) {
                if (TABLE[i].KIND == PARAMETER && TABLE[i].vp.LEVEL == LEV) {
                    // Debug: BLOCK LOOP check (removed)
                }
            }
        }
        */

        if (SYM == CONSTSYM)
        {
            GetSym();
            do
            {
                ConstDeclaration(LEV, TX, DX);
                while (SYM == COMMA)
                {
                    GetSym();
                    ConstDeclaration(LEV, TX, DX);
                }
                if (SYM == SEMICOLON)
                    GetSym();
                else
                    Error(5);
            } while (SYM == IDENT);
        }
        if (SYM == VARSYM)
        {
            GetSym();
            do
            {
                VarDeclaration(LEV, TX, DX);
                while (SYM == COMMA)
                {
                    GetSym();
                    VarDeclaration(LEV, TX, DX);
                }
                if (SYM == SEMICOLON)
                    GetSym();
                else
                    Error(5);
            } while (SYM == IDENT);
        }
        if (SYM == CHARSYM)
        { // 新增字符类型变量声明处理
            do
            {
                GetSym(); // 跳过CHARSYM
                CharVarDeclaration(LEV, TX, DX);
                while (SYM == COMMA)
                {
                    GetSym();
                    CharVarDeclaration(LEV, TX, DX);
                }
                if (SYM == SEMICOLON)
                    GetSym();
                else
                    Error(5);
            } while (SYM == CHARSYM);
        }
        if (SYM == REALSYM)
        { // 新增实数类型变量声明处理
            do
            {
                GetSym(); // 跳过REALSYM
                RealVarDeclaration(LEV, TX, DX);
                while (SYM == COMMA)
                {
                    GetSym();
                    RealVarDeclaration(LEV, TX, DX);
                }
                if (SYM == SEMICOLON)
                    GetSym();
                else
                    Error(5);
            } while (SYM == REALSYM);
        }
        while (SYM == PROCSYM)
        {
            GetSym();
            int procIndex = 0; // 声明在外层作用域
            if (SYM == IDENT)
            {
                ENTER(PROCEDUR, LEV, TX, DX);
                procIndex = TX;
                GetSym();

                // 解析参数列表（为PROCEDURE添加参数支持）
                if (SYM == LPAREN)
                {
                    GetSym();
                    int paramCount = 0;
                    int savedDX = DX; // 保存当前DX
                    DX = 4;           // 参数从地址4开始
                    if (SYM != RPAREN)
                    {
                        // 解析参数 - 先收集所有参数名
                        char paramNames[10][32]; // 最多10个参数
                        int totalParams = 0;

                        if (SYM == IDENT)
                        {
                            strcpy(paramNames[totalParams], ID);
                            totalParams++;
                            GetSym();
                            while (SYM == COMMA && totalParams < 10)
                            {
                                GetSym();
                                if (SYM == IDENT)
                                {
                                    strcpy(paramNames[totalParams], ID);
                                    totalParams++;
                                    GetSym();
                                }
                                else
                                {
                                    Error(4); // IDENT EXPECTED
                                }
                            }
                        }

                        // 现在按正确的地址顺序添加参数到符号表
                        for (int p = 0; p < totalParams; p++)
                        {
                            strcpy(ID, paramNames[p]);
                            ENTER(PARAMETER, LEV + 1, TX, DX);
                            // 参数地址：第一个参数在B-totalParams，最后一个参数在B-1
                            // 因为调用时先压第一个参数，第一个参数离基址最远
                            TABLE[TX].vp.ADR = -(totalParams - p);
                            TABLE[procIndex].vp.PARAM_TYPES[p] = PARAMETER;
                        }
                        paramCount = totalParams;
                    }
                    if (SYM == RPAREN)
                        GetSym();
                    else
                        Error(22);
                    TABLE[procIndex].vp.PARAM_COUNT = paramCount;
                    // 恢复DX以供局部变量使用
                    DX = savedDX;
                }
                else
                {
                    // 无参数的过程
                    TABLE[procIndex].vp.PARAM_COUNT = 0;
                }
            }
            else
                Error(4);
            if (SYM == SEMICOLON)
                GetSym();
            else
                Error(5);

            // 生成跳转指令跳过过程体
            GEN(JMP, 0, 0);
            int jmpAddr = CX - 1;
            TABLE[procIndex].vp.ADR = CX; // 过程的实际起始地址

            int savedTX = TX; // 保存TX状态
            Block(LEV + 1, TX, SymSetAdd(SEMICOLON, FSYS));
            TX = savedTX; // 恢复TX状态

            // 回填跳转地址
            CODE[jmpAddr].A = CX;

            if (SYM == SEMICOLON)
            {
                GetSym();
                TEST(SymSetUnion(SymSetNew(IDENT, PROCSYM), STATBEGSYS), FSYS, 6);
            }
            else
                Error(5);
        }
        while (SYM == FUNCTIONSYM)
        { // 新增函数定义处理
            GetSym();
            if (SYM == IDENT)
            {
                ENTER(FUNCTION, LEV, TX, DX);
                int funcIndex = TX;
                GetSym();

                // 解析参数列表
                if (SYM == LPAREN)
                {
                    GetSym();
                    int paramCount = 0;
                    int savedDX = DX; // 保存当前DX
                    DX = 4;           // 参数从地址4开始
                    if (SYM != RPAREN)
                    {
                        // 解析参数 - 先收集所有参数名
                        char paramNames[10][32]; // 最多10个参数
                        int totalParams = 0;

                        if (SYM == IDENT)
                        {
                            strcpy(paramNames[totalParams], ID);
                            totalParams++;
                            GetSym();
                            while (SYM == COMMA && totalParams < 10)
                            {
                                GetSym();
                                if (SYM == IDENT)
                                {
                                    strcpy(paramNames[totalParams], ID);
                                    totalParams++;
                                    GetSym();
                                }
                                else
                                {
                                    Error(4); // IDENT EXPECTED
                                }
                            }
                        }

                        // 现在按正确的地址顺序添加参数到符号表
                        for (int p = 0; p < totalParams; p++)
                        {
                            strcpy(ID, paramNames[p]);
                            ENTER(PARAMETER, LEV + 1, TX, DX);
                            // 参数地址：第一个参数在B-totalParams，最后一个参数在B-1
                            // 因为调用时先压第一个参数，第一个参数离基址最远
                            TABLE[TX].vp.ADR = -(totalParams - p);
                            TABLE[funcIndex].vp.PARAM_TYPES[p] = PARAMETER;

                            // 调试输出
                            // sprintf(debug_buf, "PARAM: %s addr=%d paramIndex=%d TX=%d", TABLE[TX].NAME,
                            //         TABLE[TX].vp.ADR, p, TX);
                            // printfs(debug_buf);

                            // 立即验证地址是否正确设置
                            // sprintf(debug_buf, "VERIFY: TABLE[%d].vp.ADR=%d", TX, TABLE[TX].vp.ADR);
                            // printfs(debug_buf);
                        }
                        paramCount = totalParams;
                    }
                    if (SYM == RPAREN)
                        GetSym();
                    else
                        Error(22);
                    TABLE[funcIndex].vp.PARAM_COUNT = paramCount;
                    // 恢复DX以供局部变量使用
                    DX = savedDX;
                }

                // 解析返回类型（简单起见，默认为整数类型）
                if (SYM == SEMICOLON)
                    GetSym();
                else
                    Error(5);

                // 设置函数地址并生成跳转指令跳过函数体
                GEN(JMP, 0, 0);
                int jmpAddr = CX - 1;
                TABLE[funcIndex].vp.ADR = CX; // 函数的实际起始地址

                int savedTX = TX; // 保存TX状态

                // 在Block调用前验证参数地址（调试用）
                // for (int i = funcIndex + 1; i <= TX; i++) {
                //     if (TABLE[i].KIND == PARAMETER) {
                //         sprintf(debug_buf, "BEFORE Block: TABLE[%d].NAME=%s, ADR=%d", i, TABLE[i].NAME,
                //         TABLE[i].vp.ADR); printfs(debug_buf);
                //     }
                // }

                Block(LEV + 1, TX, SymSetAdd(SEMICOLON, FSYS));

                // 在Block调用后验证参数地址（调试用）
                // for (int i = funcIndex + 1; i <= savedTX; i++) {
                //     if (TABLE[i].KIND == PARAMETER) {
                //         sprintf(debug_buf, "AFTER Block: TABLE[%d].NAME=%s, ADR=%d", i, TABLE[i].NAME,
                //         TABLE[i].vp.ADR); printfs(debug_buf);
                //     }
                // }

                // 不恢复TX状态，保持函数参数在符号表中
                // TX = savedTX; // 注释掉这行，保持参数符号不被覆盖

                // 回填跳转地址
                // sprintf(debug_buf, "Backfill: jmpAddr=%d, CX=%d, Before: CODE[%d].A=%d", jmpAddr, CX, jmpAddr,
                // CODE[jmpAddr].A); printfs(debug_buf);
                CODE[jmpAddr].A = CX;
                // sprintf(debug_buf, "After backfill: CODE[%d].A=%d", jmpAddr, CODE[jmpAddr].A);
                // printfs(debug_buf);

                if (SYM == SEMICOLON)
                {
                    GetSym();
                    TEST(SymSetUnion(SymSetNew(IDENT, FUNCTIONSYM), STATBEGSYS), FSYS, 6);
                }
                else
                    Error(5);
            }
            else
                Error(4);
        }
        TEST(SymSetAdd(IDENT, STATBEGSYS), DECLBEGSYS, 7);
    } while (SymIn(SYM, DECLBEGSYS));

    // 调试：检查TX0和相关地址
    /*
    char debug_buf[200];
    // Debug: Block end info (removed)
    */

    // 只有当TX0不是参数时才执行CODE回填和TABLE更新
    if (TABLE[TX0].KIND != PARAMETER)
    {
        CODE[TABLE[TX0].vp.ADR].A = CX;
        TABLE[TX0].vp.ADR = CX;  /*START ADDR OF CODE*/
        TABLE[TX0].vp.SIZE = DX; /*SIZE OF DATA SEGMENT*/
    }
    else
    {
        // sprintf(debug_buf, "SKIP CODE backfill and TABLE update for PARAMETER at TX0=%d", TX0);
        // printfs(debug_buf);
    }
    GEN(INI, 0, DX);

    // 在STATEMENT调用前检查参数地址（调试用）
    // for (int i = 1; i <= TX; i++) {
    //     if (TABLE[i].KIND == PARAMETER) {
    //         sprintf(debug_buf, "BEFORE STATEMENT: TABLE[%d].NAME=%s, ADR=%d", i, TABLE[i].NAME, TABLE[i].vp.ADR);
    //         printfs(debug_buf);
    //     }
    // }

    STATEMENT(SymSetUnion(SymSetNew(SEMICOLON, ENDSYM), FSYS), LEV, TX);

    // 在STATEMENT调用后检查参数地址（调试用）
    // for (int i = 1; i <= TX; i++) {
    //     if (TABLE[i].KIND == PARAMETER) {
    //         sprintf(debug_buf, "AFTER STATEMENT: TABLE[%d].NAME=%s, ADR=%d", i, TABLE[i].NAME, TABLE[i].vp.ADR);
    //         printfs(debug_buf);
    //     }
    // }

    GEN(OPR, 0, 0); /*RETURN*/
    TEST(FSYS, SymSetNULL(), 8);
    ListCode(CX0);
} /*Block*/
//---------------------------------------------------------------------------
int MainWindow::BASE(int L, int B, int S[])
{
    int B1 = B; /*FIND BASE L LEVELS DOWN*/
    while (L > 0)
    {
        B1 = S[B1];
        L = L - 1;
    }
    return B1;
} /*BASE*/
//---------------------------------------------------------------------------
void MainWindow::Interpret()
{
    const int STACKSIZE = 500;
    int P, B, T; /*PROGRAM BASE TOPSTACK REGISTERS*/
    INSTRUCTION I;
    int S[STACKSIZE]; /*DATASTORE*/
    //  Form1->printfs("~~~ RUN PL0 ~~~");
    printfs("~~~ RUN PL0 ~~~");
    fprintf(FOUT, "~~~ RUN PL0 ~~~\n");
    T = 0;
    B = 1;
    P = 0;
    S[1] = 0;
    S[2] = 0;
    S[3] = 0;
    do
    {
        I = CODE[P];
        P = P + 1;
        switch (I.F)
        {
        case LIT:
            T++;
            S[T] = I.A;
            // {
            //     char debug_buf[200];
            //     sprintf(debug_buf, "LIT: value=%d T=%d", I.A, T);
            //     printfs(debug_buf);
            // }
            break;
        case OPR:
            switch (I.A)
            {       /*OPERATOR*/
            case 0: /*RETURN*/
                T = B - 1;
                P = S[T + 3];
                B = S[T + 2];
                break;
            case 1:
                S[T] = -S[T];
                break;
            case 2:
                T--;
                S[T] = S[T] + S[T + 1];
                break;
            case 3:
                T--;
                S[T] = S[T] - S[T + 1];
                break;
            case 4:
                T--;
                S[T] = S[T] * S[T + 1];
                break; // 整数乘法
            case 5:
                T--;
                S[T] = S[T] / S[T + 1];
                break; // 整数除法
            case 6:
                S[T] = (S[T] % 2 != 0);
                break;
            case 8:
                T--;
                S[T] = S[T] == S[T + 1];
                break;
            case 9:
                T--;
                S[T] = S[T] != S[T + 1];
                break;
            case 10:
                T--;
                S[T] = S[T] < S[T + 1];
                break;
            case 11:
                T--;
                S[T] = S[T] >= S[T + 1];
                break;
            case 12:
                T--;
                S[T] = S[T] > S[T + 1];
                break;
            case 13:
                T--;
                S[T] = S[T] <= S[T + 1];
                break;
            case 14:
                printls("", S[T]);
                fprintf(FOUT, "%d\n", S[T]);
                T--;
                break;
            case 15: /*Form1->printfs(""); fprintf(FOUT,"\n"); */
                break;
            case 16: // 读取输入
            {
                T++; // S[T]=InputBox("输入","请键盘输入：", 0).ToInt();
                bool ok;
                int in = QInputDialog::getInt(this, tr(" 输入"), tr("请键盘输入："), 0, 0, 200, 2, &ok);
                if (ok)
                {
                    S[T] = in;
                }
                else
                {
                    S[T] = 0;
                }
                printls("? ", S[T]);
                fprintf(FOUT, "? %d\n", S[T]);
            }
            break;
            case 17: // 交换栈顶两个元素 (用于FOR循环)
            {
                int temp = S[T];
                S[T] = S[T - 1];
                S[T - 1] = temp;
            }
            break;
            case 18: // 弹出栈顶元素 (用于清理FOR循环的终止值)
                T--;
                break;
            case 19: // 复制栈顶值 (用于FOR循环保持终止值)
                T++;
                S[T] = S[T - 1];
                break;
            case 20: // 字符输出
            {
                char c = (char)S[T];
                printf("%c", c);
                string charStr(1, c);
                printfs(charStr.c_str());
                fprintf(FOUT, "%c\n", c);
                T--;
            }
            break;
            case 21: // 字符输入
            {
                T++;
                bool ok;
                QString input =
                    QInputDialog::getText(this, tr("字符输入"), tr("请输入一个字符："), QLineEdit::Normal, "", &ok);
                if (ok && !input.isEmpty())
                {
                    S[T] = (int)input.at(0).toLatin1();
                }
                else
                {
                    S[T] = 0;
                }
                char c = (char)S[T];
                printf("? %c\n", c);
                string charStr = "? " + string(1, c);
                printfs(charStr.c_str());
                fprintf(FOUT, "? %c\n", c);
            }
            break;
            case 22: // 实数输出 (从放大1000倍的整数还原为实数)
            {
                double realValue = S[T] / 1000.0;
                char buffer[50];
                sprintf(buffer, "%.3f", realValue);
                printfs(buffer);
                fprintf(FOUT, "%.3f\n", realValue);
                T--;
            }
            break;
            case 23: // 实数输入
            {
                T++;
                bool ok;
                double realInput = QInputDialog::getDouble(this, tr("实数输入"), tr("请输入一个实数："), 0.0,
                                                           -1000000.0, 1000000.0, 3, &ok);
                if (ok)
                {
                    S[T] = (int)(realInput * 1000); // 放大1000倍存储为整数
                }
                else
                {
                    S[T] = 0;
                }
                char buffer[50];
                sprintf(buffer, "? %.3f", realInput);
                printfs(buffer);
                fprintf(FOUT, "? %.3f\n", realInput);
            }
            break;
            }
            break;
        case LOD:
            T++;
            {
                int addr;
                if (I.A < 0)
                {
                    // 负偏移表示访问参数，相对于当前基址
                    addr = B + I.A;
                }
                else
                {
                    // 正偏移表示访问局部变量
                    addr = BASE(I.L, B, S) + I.A;
                }
                // char debug_buf[200];
                // sprintf(debug_buf, "LOD: L=%d A=%d B=%d base=%d addr=%d value=%d", I.L, I.A, B, BASE(I.L, B, S),
                // addr,
                //         S[addr]);
                // printfs(debug_buf);
                S[T] = S[addr];
            }
            break;
        case STO:
            S[BASE(I.L, B, S) + I.A] = S[T];
            T--;
            break;
        case CAL: /*GENERAT NEW Block MARK*/
            // 创建新的栈帧
            // 参数已经在栈上，新的基址指针应该指向参数之后
            S[T + 1] = BASE(I.L, B, S); // 静态链
            S[T + 2] = B;               // 动态链（旧的基址）
            S[T + 3] = P;               // 返回地址
            B = T + 1;                  // 新的基址指针
            P = I.A;                    // 跳转到函数入口
            break;
        case INI:
            T = T + I.A;
            // {
            //     char debug_buf[200];
            //     sprintf(debug_buf, "INI: A=%d new_T=%d", I.A, T);
            //     printfs(debug_buf);
            // }
            break;
        case JMP:
            P = I.A;
            break;
        case JPC:
            if (S[T] == 0)
                P = I.A;
            T--;
            break;
        case RET: // 新增函数返回指令处理
        {
            if (I.A == 1) // 有返回值的函数
            {
                // 带返回值的函数返回
                int returnValue = S[T]; // 保存返回值
                T = B - 1;
                P = S[T + 3];
                B = S[T + 2];
                T++;
                S[T] = returnValue; // 将返回值放到新的栈顶
            }
            else
            {
                // 无返回值的过程返回（与OPR 0类似）
                T = B - 1;
                P = S[T + 3];
                B = S[T + 2];
            }
        }
        break;
        } /*switch*/
    } while (P != 0);
    //  Form1->printfs("~~~ END PL0 ~~~");
    printfs("~~~ END PL0 ~~~");
    fprintf(FOUT, "~~~ END PL0 ~~~\n");
} /*Interpret*/
//---------------------------------------------------------------------------

void MainWindow::runClicked()
{
    for (CH = ' '; CH <= '^'; CH++)
        SSYM[(unsigned char)CH] = NUL;

    strcpy(KWORD[1], "BEGIN");
    strcpy(KWORD[2], "CALL");
    strcpy(KWORD[3], "CHAR"); // 新增字符类型关键字
    strcpy(KWORD[4], "CONST");
    strcpy(KWORD[5], "DO");
    strcpy(KWORD[6], "DOWNTO");
    strcpy(KWORD[7], "ELSE");
    strcpy(KWORD[8], "END");
    strcpy(KWORD[9], "FOR");
    strcpy(KWORD[10], "FUNCTION"); // 新增函数关键字
    strcpy(KWORD[11], "IF");
    strcpy(KWORD[12], "ODD");
    strcpy(KWORD[13], "PROCEDURE");
    strcpy(KWORD[14], "PROGRAM");
    strcpy(KWORD[15], "READ");
    strcpy(KWORD[16], "READC");    // 字符输入关键字
    strcpy(KWORD[17], "READREAL"); // 实数输入关键字
    strcpy(KWORD[18], "REAL");     // 实数类型关键字
    strcpy(KWORD[19], "RETURN");
    strcpy(KWORD[20], "STEP");
    strcpy(KWORD[21], "THEN");
    strcpy(KWORD[22], "TO");
    strcpy(KWORD[23], "UNTIL");
    strcpy(KWORD[24], "VAR");
    strcpy(KWORD[25], "WHILE");
    strcpy(KWORD[26], "WRITE");
    strcpy(KWORD[27], "WRITEC");    // 字符输出关键字
    strcpy(KWORD[28], "WRITEREAL"); // 实数输出关键字

    // 对应的符号数组
    WSYM[1] = BEGINSYM;
    WSYM[2] = CALLSYM;
    WSYM[3] = CHARSYM; // 新增字符类型符号
    WSYM[4] = CONSTSYM;
    WSYM[5] = DOSYM;
    WSYM[6] = DOWNTOSYM;
    WSYM[7] = ELSESYM;
    WSYM[8] = ENDSYM;
    WSYM[9] = FORSYM;
    WSYM[10] = FUNCTIONSYM; // 新增函数符号
    WSYM[11] = IFSYM;
    WSYM[12] = ODDSYM;
    WSYM[13] = PROCSYM;
    WSYM[14] = PROGSYM;
    WSYM[15] = READSYM;
    WSYM[16] = READCSYM;    // 字符输入符号
    WSYM[17] = READREALNUM; // 实数输入符号
    WSYM[18] = REALSYM;     // 实数类型符号
    WSYM[19] = RETURNSYM;
    WSYM[20] = STEPSYM;
    WSYM[21] = THENSYM;
    WSYM[22] = TOSYM;
    WSYM[23] = UNTILSYM;
    WSYM[24] = VARSYM;
    WSYM[25] = WHILESYM;
    WSYM[26] = WRITESYM;
    WSYM[27] = WRITECSYM;    // 字符输出符号
    WSYM[28] = WRITEREALNUM; // 实数输出符号

    // 注释掉需要复合运算符处理的字符，让 GetSym() 函数统一处理
    // SSYM['+']=PLUS;      SSYM['-']=MINUS;  // 需要处理 +=, -=, ++, --
    // SSYM['*']=TIMES;     SSYM['/']=SLASH;  // 需要处理 *=, /=
    // SSYM['|']=...;       // 需要处理 ||，单独的 | 应该报错

    // 确保这些字符被设置为NUL，以便触发GetSym()中的特殊处理
    SSYM['+'] = NUL;
    SSYM['-'] = NUL;
    SSYM['*'] = NUL;
    SSYM['/'] = NUL;
    SSYM['|'] = NUL; // 单独的 | 应该报错

    SSYM['('] = LPAREN;
    SSYM[')'] = RPAREN;
    SSYM['='] = EQL;
    SSYM[','] = COMMA;
    SSYM['.'] = PERIOD; // SSYM['#']=NEQ;	注释掉，# 现在是非法字符
    SSYM[';'] = SEMICOLON;
    SSYM['&'] = ANDSYM; // 可以保留，因为只有单一形式
    // SSYM['!']=NOTSYM;  // 注释掉，! 单独使用时为非法，只有 != 合法
    SSYM['!'] = NUL; // 强制设置为NUL，避免意外的符号识别

    strcpy(MNEMONIC[LIT], "LIT");
    strcpy(MNEMONIC[OPR], "OPR");
    strcpy(MNEMONIC[LOD], "LOD");
    strcpy(MNEMONIC[STO], "STO");
    strcpy(MNEMONIC[CAL], "CAL");
    strcpy(MNEMONIC[INI], "INI");
    strcpy(MNEMONIC[JMP], "JMP");
    strcpy(MNEMONIC[JPC], "JPC");
    strcpy(MNEMONIC[RET], "RET"); // 新增函数返回指令

    DECLBEGSYS = (int*)malloc(sizeof(int) * SYMNUM);
    STATBEGSYS = (int*)malloc(sizeof(int) * SYMNUM);
    FACBEGSYS = (int*)malloc(sizeof(int) * SYMNUM);
    for (int j = 0; j < SYMNUM; j++)
    {
        DECLBEGSYS[j] = 0;
        STATBEGSYS[j] = 0;
        FACBEGSYS[j] = 0;
    }
    DECLBEGSYS[CONSTSYM] = 1;
    DECLBEGSYS[VARSYM] = 1;
    DECLBEGSYS[CHARSYM] = 1; // 支持字符类型声明
    DECLBEGSYS[REALSYM] = 1; // 支持实数类型声明
    DECLBEGSYS[PROCSYM] = 1;
    DECLBEGSYS[FUNCTIONSYM] = 1; // 支持函数声明
    STATBEGSYS[BEGINSYM] = 1;
    STATBEGSYS[CALLSYM] = 1;
    STATBEGSYS[IFSYM] = 1;
    STATBEGSYS[WHILESYM] = 1;
    STATBEGSYS[FORSYM] = 1; // 添加FOR语句支持
    STATBEGSYS[WRITESYM] = 1;
    STATBEGSYS[WRITECSYM] = 1;    // 添加字符输出语句支持
    STATBEGSYS[WRITEREALNUM] = 1; // 添加实数输出语句支持
    STATBEGSYS[READCSYM] = 1;     // 添加字符输入语句支持
    STATBEGSYS[READREALNUM] = 1;  // 添加实数输入语句支持
    // 添加新的运算符到语句开始符号集
    // PLUSBECOMES 和 MINUSBECOMES 不需要在这里，因为它们跟在 IDENT 后面
    STATBEGSYS[PLUSPLUS] = 1;
    STATBEGSYS[MINUSMINUS] = 1;
    STATBEGSYS[TIMESBECOMES] = 1;
    STATBEGSYS[SLASHBECOMES] = 1;
    STATBEGSYS[ANDSYM] = 1;
    STATBEGSYS[ORSYM] = 1;
    // STATBEGSYS[NOTSYM]=1;  // 移除，因为 ! 单独使用时为非法
    FACBEGSYS[IDENT] = 1;
    FACBEGSYS[NUMBER] = 1;
    FACBEGSYS[REALNUMBER] = 1; // 支持实数常量
    FACBEGSYS[CHARACTER] = 1;  // 支持字符常量
    FACBEGSYS[LPAREN] = 1;
    FACBEGSYS[PLUSPLUS] = 1;   // 支持前缀++
    FACBEGSYS[MINUSMINUS] = 1; // 支持前缀--

    const string filename = sourceLineEdit->text().toStdString();
    // 使用新的文件查找功能
    QString pl0FilePath = findPL0File(QString::fromStdString(filename));
    const string pl0name = pl0FilePath.toStdString();
    const string codname = filename + ".COD";

    // 如果找到文件，设置工作目录为文件所在目录
    if (QFile::exists(pl0FilePath))
    {
        QFileInfo fileInfo(pl0FilePath);
        QString fileDir = fileInfo.absolutePath();
        QDir::setCurrent(fileDir);
        printfs(("=== 找到文件: " + pl0name).c_str());
        printfs(("=== 工作目录: " + fileDir.toStdString()).c_str());
    }

    if ((FIN = fopen(pl0name.c_str(), "r")) != 0)
    {
        FOUT = fopen(codname.c_str(), "w");
        // Form1->printfs("=== COMPILE PL0 ===");
        printfs("=== COMPILE PL0 ===");
        fprintf(FOUT, "=== COMPILE PL0 ===\n");
        ERR = 0;
        CC = 0;
        CX = 0;
        LL = 0;
        CH = ' ';
        GetSym();
        if (SYM != PROGSYM)
            Error(0);
        else
        {
            GetSym();
            if (SYM != IDENT)
                Error(0);
            else
            {
                GetSym();
                if (SYM != SEMICOLON)
                    Error(5);
                else
                    GetSym();
            }
        }
        Block(0, 0, SymSetAdd(PERIOD, SymSetUnion(DECLBEGSYS, STATBEGSYS)));
        if (SYM != PERIOD)
        {
            Error(9);
        }
        else
        {
            // 程序正常结束，关闭源文件
            fclose(FIN);
            FIN = NULL;
        }
        if (ERR == 0)
            Interpret();
        else
        {
            // Form1->printfs("ERROR IN PL/0 PROGRAM");
            printfs("ERROR IN PL/0 PROGRAM");
            fprintf(FOUT, "ERROR IN PL/0 PROGRAM");
        }
        fprintf(FOUT, "\n");
        fclose(FOUT);
    }
}
