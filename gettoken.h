#ifndef LANGUAGE_FORMAT_PROCESSING_TOOL_GETTOKEN_H
#define LANGUAGE_FORMAT_PROCESSING_TOOL_GETTOKEN_H

typedef enum token_kind{
    ERROR_TOKEN=1,
    IDENT,//标识符
    INT_CONST,//整形常量
    FLOAT_CONST,
    CHAR_CONST,
    STRING_CONST,
    KEYWORD,
    INT,
    FLOAT,
    CHAR,
    LONG,
    SHORT,
    DOUBLE,
    IF,
    ELSE,
    DO,
    WHILE,
    FOR,
    STRUCT,
    BREAK,
    SWITCH,
    CASE,
    TYPEDEF,
    RETURN,
    CONTINUE,
    VOID,//26
    LB,//左大括号
    RB,//右大括号
    LM,//左中括号
    RM,//右中括号
    SEMI,//分号31
    COMMA,//逗号
    /*EQ到MINUSMINUS为运算符，必须连在一起*/
    EQ,//'=='
    NEQ,//‘!=’
    ASSIGN,//'='35
    LP,//左括号
    RP,//右括号
    TIMES,//乘法
    DIVIDE,//除法
    PLUS,//加法40
    PLUSPLUS,//自增运算
    POUND,//井号42
    MORE,//大于号
    LESS,//小于号
    MOREEQUAL,//大于等于
    LESSEQUAL,//小于等于
    MINUS,//减法
    MINUSMINUS,//自减运算
    ANNO,//注释
    INCLUDE,//头文件引用
    MACRO,//宏定义
    ARRAY,//数组
}token_kind;

//关键字查找表
static char* KeyWords[]={
        "struct","break","else",
        "switch","case","typedef",
        "return","continue",
        "for","void","do","if","while"
};

int isNum(char c);
int isLetter(char c);
int isLorN(char c);
int gettoken(FILE*);

#endif //LANGUAGE_FORMAT_PROCESSING_TOOL_GETTOKEN_H
