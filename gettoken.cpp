#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gettoken.h"

char token_text[20];//存放单词自身值
char string_num[20];//存放数字的字符串
int cnt_lines=1;

/*判断字符是否是数字*/
int isNum(char c){
    if(c>=48&&c<=57){
        return 1;
    } else{
        return 0;
    }
}

/*判断字符是否是十六进制数*/
int isXnum(char c){
    if((isNum(c))||(c>='a'&&c<='f')||(c>='A'&&c<='F')){
        return 1;
    } else{
        return 0;
    }
};

/*判断字符是否是字母*/
int isLetter(char c){
    if((c>=65&&c<=90)||(c>=97&&c<=122)){
        return 1;
    } else{
        return 0;
    }
}

/*判断字符是否是字母或数字*/
int isLorN(char c){
    if((c>=65&&c<=90)||(c>=97&&c<=122)||(c>=48&&c<=57)){
        return 1;
    } else{
        return 0;
    }
}

/*向字符数组中添加字符*/
int add2token(char* token,char c){
    int i=0;
    while(*(token+i)!='\0'){
        i++;
    }
    /*读取的单词长度不能超过20*/
    if(i>=19){
        return -1;//添加失败
    }
    *(token+i)=c;
    *(token+i+1)='\0';
    return 1;
}

int gettoken(FILE* fp){
    int i;
    char c='0';
    *(token_text)='\0';
    /*过滤开头的空白字符*/
    do{
        c=fgetc(fp);
        if(c=='\n'){
            cnt_lines++;
        }
    }while (c==' '||c=='\n');
    //识别标识符or关键字or数组
    if(isLetter(c)||c=='_') {
        do {
            add2token(token_text, c);
        } while (isLorN(c = fgetc(fp)));
        ungetc(c, fp);//退回多读的字符到文件缓冲区
        /*先判断是否是类型关键字*/
        if(strcmp(token_text,"int")==0){
            return INT;
        }
        if(strcmp(token_text,"double")==0){
            return DOUBLE;
        }
        if(strcmp(token_text,"char")==0){
            return CHAR;
        }
        if(strcmp(token_text,"short")==0){
            return SHORT;
        }
        if(strcmp(token_text,"long")==0){
            return LONG;
        }
        if(strcmp(token_text,"float")==0){
            return FLOAT;
        }
        /*判断是否是某种关键字*/
        if(strcmp(token_text,"if")==0){
            return IF;
        }
        if(strcmp(token_text,"else")==0){
            return ELSE;
        }
        if(strcmp(token_text,"do")==0){
            return DO;
        }
        if(strcmp(token_text,"while")==0){
            return WHILE;
        }
        if(strcmp(token_text,"for")==0){
            return FOR;
        }
        if(strcmp(token_text,"struct")==0){
            return STRUCT;
        }
        if(strcmp(token_text,"break")==0){
            return BREAK;
        }
        if(strcmp(token_text,"switch")==0){
            return SWITCH;
        }
        if(strcmp(token_text,"case")==0){
            return CASE;
        }
        if(strcmp(token_text,"typedef")==0){
            return TYPEDEF;
        }
        if(strcmp(token_text,"return")==0){
            return RETURN;
        }
        if(strcmp(token_text,"continue")==0){
            return CONTINUE;
        }
        if(strcmp(token_text,"void")==0){
            return VOID;
        }
        for (i = 0; i < 13; i++) {
            if (strcmp(token_text, KeyWords[i]) == 0) {
                return KEYWORD;
            } else {
                c=fgetc(fp);
                if(c=='['){
                    //识别数组
                    add2token(token_text,c);
                    c=fgetc(fp);
                    while(c>='0'&&c<='9'){
                        add2token(token_text,c);
                        add2token(string_num,c);
                        c=fgetc(fp);
                    }
                    if(c!=']'){
                        return ERROR_TOKEN;
                    }
                    add2token(token_text,c);
                    return ARRAY;
                } else{
                    ungetc(c,fp);
                    return IDENT;
                }
            }
        }
    }
    //识别整形常量和浮点型常量
    if(isNum(c)){
        do{
            add2token(token_text,c);
        }while (isNum(c=fgetc(fp)));
        if (c!='.'&&c!='u'&&c!='l') {
            if(c!=' '&&c!=';'&&c!=')'&&c!='+'&&c!='-'&&c!='*'&&c!='/'){
                return ERROR_TOKEN;
            }
            ungetc(c,fp);
            return INT_CONST;
        } else if(c=='.'){
            c=fgetc(fp);
            if(!isNum(c)){
                return ERROR_TOKEN;
            }else{
                ungetc(c,fp);
                c='.';
                add2token(token_text,c);
                c=fgetc(fp);
                do{
                    add2token(token_text,c);
                }while (isNum(c=fgetc(fp)));
                if(c=='f'){
                    add2token(token_text,c);
                    return FLOAT_CONST;
                } else{
                    ungetc(c,fp);
                }
                return FLOAT_CONST;
            }
        }
        else if(c=='u'){
            add2token(token_text,c);
            c=fgetc(fp);
            if(c=='l'){
                add2token(token_text,c);
                c=fgetc(fp);
                if(c=='l'){
                    add2token(token_text,c);
                    return INT_CONST;
                } else{
                    ungetc(c,fp);
                    return INT_CONST;
                }
            } else{
                ungetc(c,fp);
                return INT_CONST;
            }
        }else if(c=='l'){
            add2token(token_text,c);
            return INT_CONST;
        }
        else{
            return ERROR_TOKEN;
        }
    }

    //识别以小数点开头的浮点型常量
    if(c=='.'){
        do{
            add2token(token_text,c);
        }while (isNum(c=fgetc(fp)));
        ungetc(c,fp);
        return FLOAT_CONST;
    }

    //识别字符型常量
    if(c=='\''){
        add2token(token_text,'\'');
        if((c=fgetc(fp))!='\\'){
            add2token(token_text,c);
            if((c=fgetc(fp))=='\''){
                add2token(token_text,c);
                return CHAR_CONST;
            } else{
                return ERROR_TOKEN;
            }
        } else{
            /*转义字符*/
            add2token(token_text,'\\');
            c=fgetc(fp);
            if(c=='n'||c=='t'||c=='\\'||c=='\''||c=='\"'){
                /*普通转义字符*/
                add2token(token_text,c);
                if((c=fgetc(fp))=='\''){
                    add2token(token_text,c);
                    return CHAR_CONST;
                } else{
                    return ERROR_TOKEN;
                }
            }else if(c=='x'){
                /*十六进制数转义字符*/
                add2token(token_text,c);
                if(isXnum((c=fgetc(fp)))){
                    add2token(token_text,c);
                    if(isXnum((c=fgetc(fp)))){
                        add2token(token_text,c);
                    } else{
                        ungetc(c,fp);
                    }
                    if ((c=fgetc(fp))=='\''){
                        add2token(token_text,'\'');
                        return CHAR_CONST;
                    } else{
                        return ERROR_TOKEN;
                    }
                } else{
                    return ERROR_TOKEN;
                }
            } else if(c>='0'&&c<='7'){
                /*八进制数转义字符*/
                add2token(token_text,c);
                if((c=fgetc(fp))>='0'&&c<='7'){
                    add2token(token_text,c);
                    if((c=fgetc(fp))>='0'&&c<='7'){
                        add2token(token_text,c);
                        if((c=fgetc(fp))=='\''){
                            add2token(token_text,'\'');
                            return CHAR_CONST;
                        } else{
                            return ERROR_TOKEN;
                        }
                    } else if(c=='\''){
                        add2token(token_text,'\'');
                        return CHAR_CONST;
                    } else{
                        return ERROR_TOKEN;
                    }
                } else{
                    if(c=='\''){
                        add2token(token_text,c);
                        return CHAR_CONST;
                    } else{
                        ungetc(c,fp);
                        return ERROR_TOKEN;
                    }
                }
            } else{
                return ERROR_TOKEN;
            }
        }
    }

    //识别字符串常量
    if(c=='"'){
        do{
            if(c!='\\')
                add2token(token_text,c);
            if(c=='\\'){
                c=fgetc(fp);
                add2token(token_text,c);
            }
        }while ((c=fgetc(fp))!='"'&&c!='\n');
        if(c=='"'){
            add2token(token_text,'"');
            return STRING_CONST;
        } else{
            return ERROR_TOKEN;
        }
    }

    //识别除号与注释
    if(c=='/'){
        add2token(token_text,c);
        if((c=fgetc(fp))=='/'){
            do{
                add2token(token_text,c);
            }while ((c=fgetc(fp))!='\n');
            ungetc(c,fp);
            return ANNO;
        } else if(c=='*'){
            while (1){
                add2token(token_text,c);
                c=fgetc(fp);
                if(c=='*'){
                    add2token(token_text,c);
                    if((c=fgetc(fp))=='/'){
                        add2token(token_text,c);
                        return ANNO;
                    }
                }
                if(c=='\n'){
                    add2token(token_text,c);
                    c='\t';
                    add2token(token_text,c);
                    add2token(token_text,c);
                }
            }
        } else{
            ungetc(c,fp);
            return DIVIDE;
        }
    }

    //识别头文件引用和宏定义
    if(c=='#'){
        add2token(token_text,c);
        if(isLetter(c=fgetc(fp))){
            do{
                add2token(token_text,c);
            }while (isLetter(c=fgetc(fp)));
            if(strcmp(token_text,"#include")==0){
                do{
                    add2token(token_text,c);
                }while ((c=fgetc(fp))!='\n');
                ungetc(c,fp);
                return INCLUDE;
            } else if(strcmp(token_text,"#define")==0){
                do{
                    add2token(token_text,c);
                }while ((c=fgetc(fp))!='\n');
                return MACRO;
            } else{
                return ERROR_TOKEN;
            }
        } else{
            return ERROR_TOKEN;
        }
    }

    if(c=='@'||c=='?'){//一些非法字符
        return ERROR_TOKEN;
    }

    switch (c){
        case ',':
            add2token(token_text,c);
            return COMMA;
        case ';':
            add2token(token_text,c);
            return SEMI;
        case '=':
            c=fgetc(fp);
            if(c=='='){
                add2token(token_text,c);
                add2token(token_text,c);
                return EQ;
            }
            ungetc(c,fp);
            add2token(token_text,'=');
            return ASSIGN;
        case '!':
            c=fgetc(fp);
            if(c=='='){
                add2token(token_text,'!');
                add2token(token_text,'=');
                return NEQ;
            } else{
                return ERROR_TOKEN;
            }
        case '+':
            c=fgetc(fp);
            if(c=='+'){
                add2token(token_text,c);
                add2token(token_text,c);
                return PLUSPLUS;
            }
            ungetc(c,fp);
            add2token(token_text,'+');
            return PLUS;
        case '-':
            c=fgetc(fp);
            if(c=='-'){
                add2token(token_text,c);
                add2token(token_text,c);
                return MINUSMINUS;
            }
            ungetc(c,fp);
            add2token(token_text,'-');
            return MINUS;
        case '(':
            add2token(token_text,c);
            return LP;
        case ')':
            add2token(token_text,c);
            return RP;
        case '{':
            add2token(token_text,c);
            return LB;
        case '}':
            add2token(token_text,c);
            return RB;
        case '[':
            add2token(token_text,c);
            return LM;
        case ']':
            add2token(token_text,c);
            return RM;
        case '*':
            add2token(token_text,c);
            return TIMES;
        case '>':
            add2token(token_text,c);
            if((c=fgetc(fp))=='='){
                add2token(token_text,c);
                return MOREEQUAL;
            } else{
                ungetc(c,fp);
                return MORE;
            }
        case '<':
            add2token(token_text,c);
            if((c=fgetc(fp))=='='){
                add2token(token_text,c);
                return LESSEQUAL;
            } else{
                ungetc(c,fp);
                return LESS;
            }
        case '~':
            return -1;
    }
}

