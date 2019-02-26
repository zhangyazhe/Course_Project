#include <stdio.h>
#include <stdlib.h>
#include "word_analyse.h"
#include "gettoken.h"

extern char token_text[20];//存放单词自身值
extern int cnt_lines;
extern FILE* fp;

int word_analyse() {
    int kind;
    if(fp==NULL){
        printf("文件打开失败\n");
    } else{
        printf("文件打开成功\n");
    }
    printf("\n");
    printf("  单词类别");
    printf("\t单词值\n");
    do {
        kind=gettoken(fp);
        if(kind!=ERROR_TOKEN) {
            switch (kind) {
                case IDENT:
                    printf("   标识符");
                    break;
                case INT_CONST:
                    printf("  整型常量");
                    break;
                case FLOAT_CONST:
                    printf("浮点型常量");
                    break;
                case CHAR_CONST:
                    printf("  字符常量");
                    break;
                case STRING_CONST:
                    printf("字符串常量");
                    break;
                case KEYWORD:
                    printf("   关键字");
                    break;
                case INT:
                    printf("类型关键字");
                    break;
                case DOUBLE:
                    printf("类型关键字");
                    break;
                case FLOAT:
                    printf("类型关键字");
                    break;
                case CHAR:
                    printf("类型关键字");
                    break;
                case SHORT:
                    printf("类型关键字");
                    break;
                case LONG:
                    printf("类型关键字");
                    break;
                case IF:
                    printf("   关键字");
                    break;
                case ELSE:
                    printf("   关键字");
                    break;
                case DO:
                    printf("   关键字");
                    break;
                case WHILE:
                    printf("   关键字");
                    break;
                case FOR:
                    printf("   关键字");
                    break;
                case STRUCT:
                    printf("   关键字");
                    break;
                case BREAK:
                    printf("   关键字");
                    break;
                case SWITCH:
                    printf("   关键字");
                    break;
                case CASE:
                    printf("   关键字");
                    break;
                case TYPEDEF:
                    printf("   关键字");
                    break;
                case RETURN:
                    printf("   关键字");
                    break;
                case CONTINUE:
                    printf("   关键字");
                    break;
                case VOID:
                    printf("   关键字");
                    break;
                case EQ:
                    printf("   等于号");
                    break;
                case NEQ:
                    printf("  不等于号");
                    break;
                case ASSIGN:
                    printf("   赋值号");
                    break;
                case LP:
                    printf("   左括号");
                    break;
                case RP:
                    printf("   右括号");
                    break;
                case LB:
                    printf("  左大括号");
                    break;
                case RB:
                    printf("  右大括号");
                    break;
                case LM:
                    printf("  左中括号");
                    break;
                case RM:
                    printf("  右中括号");
                    break;
                case SEMI:
                    printf("     分号");
                    break;
                case COMMA:
                    printf("     逗号");
                    break;
                case TIMES:
                    printf("     乘号");
                    break;
                case DIVIDE:
                    printf("     除号");
                    break;
                case ANNO:
                    printf("     注释");
                    break;
                case PLUS:
                    printf("     加法");
                    break;
                case PLUSPLUS:
                    printf("  自增运算");
                    break;
                case MINUS:
                    printf("     减法");
                    break;
                case MINUSMINUS:
                    printf("  自减运算");
                    break;
                case MORE:
                    printf("   大于号");
                    break;
                case MOREEQUAL:
                    printf("大于等于号");
                    break;
                case LESS:
                    printf("   小于号");
                    break;
                case LESSEQUAL:
                    printf("小于等于号");
                    break;
                case INCLUDE:
                    printf("头文件引用");
                    break;
                case MACRO:
                    printf("   宏定义");
                    break;
                case ARRAY:
                    printf("     数组");
                    break;
            }
            printf("\t%s\n", token_text);
        } else{
            printf("\t第%d行出现错误\n",cnt_lines);
            break;
        }
    } while (kind!=-1);

    return 0;
}