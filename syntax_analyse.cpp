#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stack>
#include "syntax_analyse.h"
#include "gettoken.h"

using namespace std;

extern char token_text[20];//存放自身单词值
extern char string_num[20];
extern int cnt_lines;
int w,type;//全局变量，存放当前读入的单词种类编码
int mistake=0;//全局变量，有任何一个未知出错都会导致其值变为1
extern FILE* fp;
VDN* Vroot;//变量名链表根节点
int isVoid,hasReturn,isInRecycle=0;


void syntax_analyse(){
    ASTTree* root=program();
    if(root==NULL||mistake==1){
        printf("程序语法错误\n");
        return;
    } else{
        printf("代码正确\n\n");
        /*呈现语法树*/
        /*递归先序遍历*/
        PreorderTranverse(root,0);
    }
}

ASTTree* program(){
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    Vroot=(VDN*)malloc(sizeof(VDN));
    Vroot->size=0;
    Vroot->next=NULL;
    ASTTree *p=ExtDefList();
    if(p!=NULL){
        if(isVoid==0&&hasReturn==0){
            printf("错误：函数缺少返回值\n");
            exit(0);
        }
        /*程序语法正确，返回语法树根节点指针，可遍历显示*/
        ASTTree* root=p;//外部定义序列结点
        root->type=EXTDEFLIST;
        return root;
    } else{
        //有语法错误
        mistake=1;
        return NULL;
    }
}

/*释放root为根的全部结点*/
void freeTree(ASTTree* root){
    if(root){
        freeTree(root->l);
        freeTree(root->r);
        free(root);
    }
}

ASTTree* ExtDefList(){//处理外部定义序列
    if(mistake==1){
        return NULL;
    }
    if(w==-1){
        return NULL;
    }
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//外部定义序列结点
    root->data.data=NULL;
    root->type=EXTDEFLIST;
    root->l=ExtDef();
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    root->r=ExtDefList();
    return root;
}

/*语法单位<外部定义>子程序*/
//调用前已经读入了一个单词
ASTTree* ExtDef(){
    int a;
    if(mistake==1){
        return NULL;
    }
    if(w!=INT&&w!=DOUBLE&&w!=CHAR&&w!=LONG&&w!=SHORT&&w!=FLOAT&&w!=VOID){
        printf("第%d行出现错误\n",cnt_lines);//%
        printf("错误：外部定义出现错误\n");
        exit(0);
    }
    type=w;//保存类型说明符
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    if(w!=IDENT&&w!=ARRAY){
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：外部定义出现错误\n");
        exit(0);
    }
    a=w;
    char token_text0[20];
    strcpy(token_text0,token_text);//保存第一个变量名或者函数名到token_text0
    ASTTree* p;
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    strcpy(token_text,token_text0);
    if(w==LP){
        p=FuncDef();
    }else if(a==ARRAY){//数组定义，读完了整个数组读到了分号
        p=ArrayDef();
    } else{
        p=ExtVarDef();
    }
    return p;
}

/*处理数组定义的子程序*/
ASTTree* ArrayDef(){
    //此时token_text中包括数组名与中括号
    if(type==VOID){
        //数组类型不能是void
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：数组类型不能是void\n");
        exit(0);
    }
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));
    root->type=ARRAYDEF;
    root->l=NULL;
    root->r=NULL;
    root->data.data=NULL;
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));
    p->type=ARRAYTYPE;
    p->l=NULL;
    p->r=NULL;
    if(type==INT){
        p->data.data="int";
    }if(type==DOUBLE){
        p->data.data="double";
    }if(type==CHAR){
        p->data.data="char";
    }if(type==FLOAT){
        p->data.data="float";
    }if(type==LONG){
        p->data.data="long";
    }if(type==SHORT){
        p->data.data="short";
    }
    root->l=p;
    p=(ASTTree*)malloc(sizeof(ASTTree));
    p->type=ARRAYNAME;
    p->l=NULL;
    p->r=NULL;
    char* token_text3=(char*)malloc(20*sizeof(char));
    strcpy(token_text3,token_text);
    p->data.data=token_text3;
    root->r=p;
    ASTTree* q=(ASTTree*)malloc(sizeof(ASTTree));
    q->type=ARRAYSIZE;
    q->l=NULL;
    q->r=NULL;
    q->data.data=string_num;
    p->l=q;
    return root;
}//退出时不能多读单词


/*处理外部变量子程序*/
ASTTree* ExtVarDef(){
    if(mistake==1){
        return NULL;
    }
    int cnt;
    if(type==VOID){
        //外部变量类型不能是void
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：外部变量类型不能是void\n");
        exit(0);
    }

    int u;
    u=addname(token_text);
    if(u==1){
        //变量被重复定义
        mistake=1;
        return NULL;
    }

    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量定义的结点
    root->l=NULL;
    root->r=NULL;
    root->data.data=NULL;
    root->type=EXTVARDEF;
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量类型结点
    p->l=NULL;
    p->r=NULL;
    p->data.data=NULL;
    p->type=EXTVARTYPE;
    p->data.type=type;
    if(type==INT){
        p->data.data="int";
    }if(type==DOUBLE){
        p->data.data="double";
    }if(type==CHAR){
        p->data.data="char";
    }if(type==FLOAT){
        p->data.data="float";
    }if(type==LONG){
        p->data.data="long";
    }if(type==SHORT){
        p->data.data="short";
    }
    root->l=p;
    p=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量名序列结点
    p->l=NULL;
    p->r=NULL;
    p->data.data=NULL;
    p->type=EXTVARLIST;
    root->r=p;
    p->l=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量名结点
    p->l->l=NULL;
    p->l->r=NULL;
    p->data.data=NULL;
    p->l->type=EXTVAR;
    char *token_text1=(char*)malloc(25* sizeof(char));//@
    strcpy(token_text1,token_text);
    p->l->data.data=token_text1;
    while(1){
        if(w!=COMMA&&w!=SEMI){
            if(cnt_lines>cnt){
                cnt_lines--;
            }
            printf("第%d行出现错误\n",cnt_lines);//%
            printf("错误：外部变量定义处出现错误\n");
            exit(0);
        }
        if(w==SEMI){
            return root;
        }
        w=gettoken(fp);
        if(w!=IDENT){
            printf("第%d行出现错误\n",cnt_lines);//%
            printf("错误：外部变量定义处出现错误\n");
            exit(0);
        }

        int u;
        u=addname(token_text);
        if(u==1){
            //变量被重复定义
            mistake=1;
            return NULL;
        }

        ASTTree* q=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量名序列结点
        q->l=NULL;
        q->r=NULL;
        q->data.data=NULL;
        q->type=EXTVARLIST;
        p->r=q;
        p=q;
        p->l=(ASTTree*)malloc(sizeof(ASTTree));//生成外部变量名结点
        p->l->l=NULL;
        p->l->r=NULL;
        p->l->data.data=NULL;
        p->l->type=EXTVAR;
        char *token_text1=(char*)malloc(25* sizeof(char));
        strcpy(token_text1,token_text);
        p->l->data.data=token_text1;
        cnt=cnt_lines;
        w=gettoken(fp);
        while(w==ANNO||w==INCLUDE){
            w=gettoken(fp);
        }
    }
}//退出函数时没有多读取单词

/*处理外部函数子程序*/
ASTTree* FuncDef(){
    if(mistake==1){
        return NULL;
    }
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));// 生成函数定义结点
    root->data.data=NULL;
    root->type=FUNCDEF;
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//生成返回值类型结点
    p->data.data=NULL;
    p->type=FUNCRETURNTYPE;
    p->data.type=type;
    if(type==INT){
        p->data.data="int";
        isVoid=0;
    }if(type==DOUBLE){
        p->data.data="double";
        isVoid=0;
    }if(type==CHAR){
        p->data.data="char";
        isVoid=0;
    }if(type==FLOAT){
        p->data.data="float";
        isVoid=0;
    }if(type==LONG){
        p->data.data="long";
        isVoid=0;
    }if(type==SHORT){
        p->data.data="short";
        isVoid=0;
    }if(type==VOID){
        p->data.data="void";
        isVoid=1;
    }
    p->l=NULL;
    p->r=NULL;
    root->l=p;
    /*处理参数*/
    ASTTree* q=(ASTTree*)malloc(sizeof(ASTTree));
    q->data.data=NULL;
    q->type=FUNCNAME;
    char *token_text1=(char*)malloc(25* sizeof(char));
    strcpy(token_text1,token_text);
    q->data.data=token_text1;

    //在变量名链表上生成新的结点$
    VDN* last=Vroot;
    while (last->next!=NULL){
        last=last->next;
    }
    last->next=(VDN*)malloc(sizeof(VDN));
    last=last->next;
    last->next=NULL;
    last->size=0;

    root->r=q;
    q->l=FormParaList();
    /*判断是何种函数定义*/
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    if(w==SEMI){
        /*函数原型声明*/
        root->r->r=NULL;//函数体结点为空
        root->type=FUNCCLAIM;
    }
    else if(w==LB){
        //函数体（复合语句）子程序
        q->r=CompState();
        q->r->type=FUNCBODY;
    } else{
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：函数定义处出错\n");
        exit(0);
    }
    return root;
}

/*处理形式参数子程序*/
ASTTree* FormParaList(){
    if(mistake==1){
        return NULL;
    }
    //第一次进入此函数之前，已经识别到了左括号
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    if(w==RP){
        return NULL;
    }
    if(w==COMMA){
        w=gettoken(fp);
        while(w==ANNO||w==INCLUDE){
            w=gettoken(fp);
        }
    }
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//生成形式参数序列结点
    root->data.data=NULL;
    root->type=FUNCFORMALPARALIST;
    root->l=FormParaDef();
    root->r=FormParaList();
    return root;
}

ASTTree* FormParaDef(){
    if(mistake==1){
        return NULL;
    }
    if(w!=INT&&w!=DOUBLE&&w!=CHAR&&w!=LONG&&w!=SHORT&&w!=FLOAT){
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：形参定义处出现错误\n");
        exit(0);
    }
    type=w;//保存类型说明符
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    if(w!=IDENT){
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：形参定义处出现错误\n");
        exit(0);
    }
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//生成形式参数定义的结点
    root->data.data=NULL;
    root->type=FUNCFORMALPARADEF;
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//生成形参类型结点
    p->data.data=NULL;
    p->type=FUNCFORMALPARATYPE;
    p->data.type=type;
    if(type==INT){
        p->data.data="int";
    }if(type==DOUBLE){
        p->data.data="double";
    }if(type==CHAR){
        p->data.data="char";
    }if(type==FLOAT){
        p->data.data="float";
    }if(type==LONG){
        p->data.data="long";
    }if(type==SHORT){
        p->data.data="short";
    }if(type==VOID){
        p->data.data="void";
    }
    p->l=NULL;
    p->r=NULL;
    root->l=p;
    p=(ASTTree*)malloc(sizeof(ASTTree));//生成形参名结点
    p->data.data=NULL;
    p->type=FUNCFORMALPARA;

    int u;
    u=addname(token_text);
    if(u==1){
        //变量被重复定义
        mistake=1;
        return NULL;
    }

    char *token_text1=(char*)malloc(25* sizeof(char));
    strcpy(token_text1,token_text);
    p->data.data=token_text1;//记录形参名
    p->l=NULL;
    p->r=NULL;
    root->r=p;
    return root;
}

/*复合语句子程序*/
ASTTree* CompState(){
    if(mistake==1){
        return NULL;
    }
    /*调用此子程序时，已经读入了单词{，继续处理时，如果遇到}，结束复合语句*/
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//生成复合语句结点
    root->data.data=NULL;
    root->l=NULL;
    root->r=NULL;
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    if(w==INT||w==DOUBLE||w==CHAR||w==LONG||w==SHORT||w==FLOAT){
        root->l=LocalVarDefList();
    } else{
        /*无局部变量声明*/
        root->l=NULL;
    }
    /*调用处理语句序列子程序*/
    root->r=StateList();
    if(w==RB){
        //遇到右大括号结束
        return root;
    } else{
        printf("错误：处理复合语句出错\n");
        exit(0);
        mistake=1;
        freeTree(root);
        return NULL;
    }

}

/*局部变量定义子程序*/
ASTTree* LocalVarDefList(){
    if(mistake==1){
        return NULL;
    }
    /*仅读取一行，到分号结束*/
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));//局部变量定义序列结点
    root->data.data=NULL;
    root->type=LOCALVARDEFLIST;
    root->l=NULL;
    root->r=NULL;
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//局部变量定义结点
    p->data.data=NULL;
    p->type=LOCALVARDEF;
    p->l=NULL;
    p->r=NULL;
    root->l=p;
    p->l=(ASTTree*)malloc(sizeof(ASTTree));//局部变量类型结点
    p->l->data.data=NULL;
    p->l->type=LOCALVARTYPE;
    char *token_text1=(char*)malloc(25* sizeof(char));
    strcpy(token_text1,token_text);
    p->l->data.data=token_text1;
    p->l->l=NULL;
    p->l->r=NULL;
    w=gettoken(fp);
    while(w==ANNO||w==INCLUDE){
        w=gettoken(fp);
    }
    ASTTree* q=(ASTTree*)malloc(sizeof(ASTTree));//变量名序列结点
    q->data.data=NULL;
    q->l=NULL;
    q->r=NULL;
    p->r=q;
    q->type=LOCALVARNAMELIST;
    q->l=(ASTTree*)malloc(sizeof(ASTTree));//局部变量名结点
    q->l->data.data=NULL;
    q->l->type=LOCALVARNAME;
    char *token_text2=(char*)malloc(25* sizeof(char));
    strcpy(token_text2,token_text);
    q->l->data.data=token_text2;
    q->l->l=NULL;
    q->l->r=NULL;

    int u;
    u=addname(token_text);
    if(u==1){
        //变量被重复定义
        mistake=1;
        return NULL;
    }

    while (1){
        w=gettoken(fp);
        while(w==ANNO||w==INCLUDE){
            w=gettoken(fp);
        }
        if(w==SEMI){
            /*局部变量定义结束*/
            q->r=NULL;
            w=gettoken(fp);//多读一个
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            break;
        }
        else if(w==COMMA){
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            ASTTree* s=(ASTTree*)malloc(sizeof(ASTTree));//变量名序列结点
            s->data.data=NULL;
            q->r=s;
            q=q->r;
            q->type=LOCALVARNAMELIST;
            q->l=(ASTTree*)malloc(sizeof(ASTTree));//局部变量名结点
            q->l->data.data=NULL;
            q->l->type=LOCALVARNAME;
            char *token_text1=(char*)malloc(25* sizeof(char));
            strcpy(token_text1,token_text);
            q->l->data.data=token_text1;

            int u;
            u=addname(token_text);
            if(u==1){
                //变量被重复定义
                freeTree(root);
                mistake=1;
                return NULL;
            }
        } else{
            printf("第%d行出现错误\n",cnt_lines);//%
            printf("错误：局部变量定义出错\n");
            exit(0);
        }
    }
    /*局部变量定义可能不只一行*/
    if(w==INT||w==DOUBLE||w==CHAR||w==LONG||w==SHORT||w==FLOAT){
        root->r=LocalVarDefList();
    } else{
        root->r=NULL;
    }
    root->r=NULL;
    return root;
    //此函数结束时是多读了一个词的
}

/*<语句序列>子程序*/
/*处理前已经读入语句序列的第一个单词*/
ASTTree* StateList(){
    if(mistake==1){
        return NULL;
    }
    ASTTree* root=NULL;
    ASTTree* r1=Statement();
    if(r1==NULL){
        //语句序列已结束或者出现错误
        return NULL;
    } else{
        root=(ASTTree*)malloc(sizeof(ASTTree));
        root->data.data=NULL;
        root->type=STATELIST;
        root->l=r1;
        root->r=NULL;
        w=gettoken(fp);
        while(w==ANNO||w==INCLUDE){
            w=gettoken(fp);
        }
        if(w!=RB){//程序还没结束
            root->r=StateList();
            return root;
        } else{//程序结束了
            return root;
        }

    }
    //程序结束时，多读取了一个单词
}

/*<处理一条语句>子程序*/
ASTTree* Statement(){
    if(mistake==1){
        return NULL;
    }
    //调用此程序时，语句的第一个单词已经读入
    ASTTree* root=(ASTTree*)malloc(sizeof(ASTTree));
    root->l=NULL;
    root->r=NULL;
    root->data.data=NULL;
    switch (w){
        case IF: {//分析条件语句
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w != LP) {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：if语句出错\n");
                exit(0);
            }
            //处理表达式的子程序
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            ASTTree *p1 = (ASTTree *) malloc(sizeof(ASTTree));
            p1->data.data=NULL;
            p1->type = IFPART;
            p1->l = Expression(RP);//处理一句表达式
            if (p1->l == NULL) {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：if语句条件部分出错\n");
                exit(0);
            }
            w = gettoken(fp);//读到左大括号
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w == LB) {
                /*if部分的语句是有大括号括住的，可以有多条语句，以右大括号结束*/
                w = gettoken(fp);
                while(w==ANNO||w==INCLUDE){
                    w=gettoken(fp);
                }
                p1->r = StateList();
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST || w == IDENT||w==KEYWORD||w==IF||w==WHILE||w==ELSE||w==FOR||w==DO) {
                /*没有大括号，此处只能有一条语句*/
                p1->r = Statement();
                p1->r->r=NULL;
            } else {
                printf("错误：if语句体出错\n");
                mistake=1;
                return NULL;
            }
            root->l = p1;//if部分处理完毕
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w == ELSE) {
                root->type = IFELSESTATEMENT;
                ASTTree *p2 = (ASTTree *) malloc(sizeof(ASTTree));
                p2->data.data=NULL;
                p2->type = ELSEPART;
                root->r = p2;
                w = gettoken(fp);
                while(w==ANNO||w==INCLUDE){
                    w=gettoken(fp);
                }
                if (w == LB) {
                    /*else部分的语句是有大括号括住的，可以有多条语句，以右大括号结束*/
                    w = gettoken(fp);
                    while(w==ANNO||w==INCLUDE){
                        w=gettoken(fp);
                    }
                    p2->r = StateList();
                } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST || w == IDENT||w==KEYWORD) {
                    /*没有大括号，此处只能有一条语句*/
                    p2->r = Statement();
                    p2->r->r=NULL;
                } else if(w==IF) {
                    p2->l=Statement();
                }else{
                    printf("错误：else子句出错\n");
                    mistake=1;
                    return NULL;
                }
            } else{
                root->type = IFSTATEMENT;
                returntoken(w,fp);

            }
            return root;
        }
        case WHILE:{
            isInRecycle=1;
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w != LP) {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：while语句出错\n");
                exit(0);
                mistake=1;
                return NULL;
            }
            //处理表达式的子程序
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            ASTTree *p1 = (ASTTree *) malloc(sizeof(ASTTree));
            p1->data.data=NULL;
            p1->type = WHILEPART;//while条件语句
            p1->l=NULL;
            p1->r=NULL;
            p1->l = Expression(RP);//处理一句表达式 while条件语句
            if (p1->l == NULL) {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：while语句条件部分出错\n");
                exit(0);
                mistake=1;
                return NULL;
            }//while条件部分处理完毕
            ASTTree* p2=(ASTTree*)malloc(sizeof(ASTTree));
            p2->data.data=NULL;
            p2->type=WHILEBODY;
            p2->l=NULL;
            p2->r=NULL;
            w = gettoken(fp);//读到左大括号
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w == LB) {
                /*while部分的语句是有大括号括住的，可以有多条语句，以右大括号结束*/
                w = gettoken(fp);
                while(w==ANNO||w==INCLUDE){
                    w=gettoken(fp);
                }
                p2->r = StateList();
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST || w == IDENT||w==KEYWORD) {
                /*没有大括号，此处只能有一条语句*/
                p2->r = Statement();
                p2->r->r=NULL;
            } else {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：while语句出错\n");
                exit(0);
                mistake=1;
                return NULL;
            }
            root->type=WHILESTATEMENT;
            root->l=p1;
            root->r=p2;
            isInRecycle=0;
            return root;
        }
        case FOR:{
            isInRecycle=1;
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w != LP) {
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：for语句出错\n");
                exit(0);
                mistake=1;
                return NULL;
            }
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            ASTTree *p1 = (ASTTree *) malloc(sizeof(ASTTree));
            p1->data.data=NULL;
            p1->type = FORPART;//for条件语句
            ASTTree* q=(ASTTree*)malloc(sizeof(ASTTree));//条件一
            p1->l=q;
            q->type=FORPART1;
            q->data.data=NULL;
            q->l=Expression(SEMI);
            if(q->l==NULL){
                q->data.data="无";
            }
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            q->r=(ASTTree*)malloc(sizeof(ASTTree));//for语句条件二
            q=q->r;
            q->type=FORPART2;
            q->data.data=NULL;
            q->l=Expression(SEMI);
            if(q->l==NULL){
                q->data.data="无";
            }
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            q->r=(ASTTree*)malloc(sizeof(ASTTree));//for语句条件三
            q=q->r;
            q->r=NULL;
            q->type=FORPART3;
            q->data.data=NULL;
            q->l=Expression(RP);
            if(q->l==NULL){
                q->data.data="无";
            }
            //for语句条件部分处理完毕
            ASTTree* p2=(ASTTree*)malloc(sizeof(ASTTree));//for语句体结点
            p2->l=NULL;
            p2->r=NULL;
            p2->type=FORBODY;
            p2->data.data=NULL;
            w = gettoken(fp);//读到左大括号
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if (w == LB) {
                /*for语句体是有大括号括住的，可以有多条语句，以右大括号结束*/
                w = gettoken(fp);
                while(w==ANNO||w==INCLUDE){
                    w=gettoken(fp);
                }
                p2->r = StateList();
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST || w == IDENT||w==KEYWORD) {
                /*没有大括号，此处只能有一条语句*/
                p2->r = Statement();
                p2->r->r=NULL;
            } else {
                printf("错误：for语句出错\n");
                exit(0);
            }
            root->type=FORSTATEMENT;
            root->l=p1;
            root->r=p2;
            isInRecycle=0;
            return root;
        }
        case RETURN:{
            hasReturn=1;
            if(isVoid==1){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：函数不应该有返回值\n");
                exit(0);
            }
            root->type=RETURNSTATEMENT;
            root->l=NULL;
            root->r=NULL;
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            root->r=Expression(SEMI);
            return root;
        }
        case DO:{
            isInRecycle=1;
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=LB){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：do-while语句缺少大括号\n");
                exit(0);
            }
            ASTTree* p1=(ASTTree*)malloc(sizeof(ASTTree));
            p1->type=DOWHILEBODY;
            p1->l=NULL;
            p1->r=NULL;
            ASTTree* p2=(ASTTree*)malloc(sizeof(ASTTree));
            p2->type=DOWHILECONDITION;
            p2->l=NULL;
            p2->r=NULL;
            root->l=p1;
            root->r=p2;
            root->data.data=NULL;
            p1->data.data=NULL;
            p2->data.data=NULL;
            w = gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            p1->l=StateList();

            //do-while语句体处理完毕
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=WHILE){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：do-while语句缺少while关键字\n");
                freeTree(root);
                exit(0);
            }
            root->type=DOWHILESTATEMENT;
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=LP){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：do-while语句缺少while条件\n");
                freeTree(root);
                exit(0);
            }
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            p2->l=Expression(RP);
            if(p2->l==NULL){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：do-while语句缺少条件\n");
                exit(0);
            }
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=SEMI){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：do-while语句缺少分号\n");
                freeTree(root);
                exit(0);
            }
            isInRecycle=0;
            return root;
        }
        case BREAK:{
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=SEMI){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：break语句缺少分号\n");
                exit(0);
            }
            if(isInRecycle==0){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：非循环语句中出现了break语句\n");
                exit(0);
            }
            root->type=BREAKSTATEMENT;
            return root;
        }
        case CONTINUE:{
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
            if(w!=SEMI){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：continue语句缺少分号\n");
                exit(0);
            }
            if(isInRecycle==0){
                printf("第%d行出现错误\n",cnt_lines);
                printf("错误：非循环语句中出现了continue语句\n");
                exit(0);
            }
            root->type=CONTINUESTATEMENT;
            return root;
        }
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
        case IDENT:
        case ARRAY:
            return Expression(SEMI);

    }
    return root;
}//程序结束时没有多读单词

/*<表达式>子程序*/
//调用前已经读入了一个单词
ASTTree* Expression(int endsym){//endsym是表达式结束符号，分号或者右小括号
    if(mistake==1){
        return NULL;
    }
    if(w==endsym){//针对for循环可能会出现语句为空的情况
        return NULL;
    }
    int error=0;
    stack<ASTTree*> op;//运算符栈
    ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//定义起止符号结点
    p->data.data=NULL;
    p->type=OPERATOR;
    p->data.type=POUND;
    op.push(p);
    stack<ASTTree*> opn;//操作数栈
    ASTTree *t,*t1,*t2,*root;
    while (((w!=endsym)||(op.top()->data.type!=POUND))&&!error){
        if(op.top()->data.type==RP){//去括号
            if(op.size()<3){
                error++;
                break;
            }
            op.pop();
            op.pop();
        }
        if(w==IDENT){
            if(checkname_exist(token_text)==0){
                mistake=1;
            }
        }
        if(w==IDENT||w==INT_CONST||w==FLOAT_CONST||w==CHAR_CONST||w==ARRAY||w==STRING_CONST){
            p=(ASTTree*)malloc(sizeof(ASTTree));
            p->data.data=NULL;
            p->type=OPERAND;
            char *token_text1=(char*)malloc(25* sizeof(char));
            strcpy(token_text1,token_text);
            p->data.data=token_text1;
            opn.push(p);
            w=gettoken(fp);
            while(w==ANNO||w==INCLUDE){
                w=gettoken(fp);
            }
        }
        else if(w==endsym){
            ASTTree* p=(ASTTree*)malloc(sizeof(ASTTree));//定义起止符号结点
            p->data.data=NULL;
            p->type=OPERATOR;
            p->data.type=POUND;
            while(op.top()->data.type!=POUND){
                t2 = opn.top();
                if (!t2&&(op.top()->data.type)!=PLUSPLUS&&(op.top()->data.type)!=MINUSMINUS) {
                    error++;
                    break;
                }
                if(t2!=NULL)
                    opn.pop();
                if (opn.size()==0) {
                    t1 = NULL;
                }else{
                    t1 = opn.top();
                }
                if (!t1&&(op.top()->data.type)!=PLUSPLUS&&(op.top()->data.type)!=MINUSMINUS) {
                    error++;
                    break;
                }
                if(t1!=NULL)
                    opn.pop();
                t = op.top();
                if (!t) {
                    error++;
                    break;
                }
                op.pop();
                t->l = t1;
                t->r = t2;
                opn.push(t);

            }
            if(opn.size()!=1){
                error++;
            }

        }
        else if(w>=EQ&&w<=MINUSMINUS) {
            char *token_text1=(char*)malloc(25* sizeof(char));//@
            switch (Precede(op.top()->data.type, w)) {
                case '<':
                    p = (ASTTree *) malloc(sizeof(ASTTree));
                    p->data.data=NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(token_text1,token_text);
                    p->data.data=token_text1;
                    op.push(p);
                    w = gettoken(fp);
                    while(w==ANNO||w==INCLUDE){
                        w=gettoken(fp);
                    }
                    break;
                case '='://去括号
                    t = op.top();
                    if (!t) {
                        error++;
                        op.pop();
                    }
                    w = gettoken(fp);
                    while(w==ANNO||w==INCLUDE){
                        w=gettoken(fp);
                    }
                    break;
                case '>':
                    t2 = opn.top();
                    if (!t2&&(op.top()->data.type)!=PLUSPLUS&&(op.top()->data.type)!=MINUSMINUS) {
                        error++;
                        break;
                    }
                    if(t2!=NULL)
                        opn.pop();
                    if (opn.size()==0) {
                        t1 = NULL;
                    }else{
                        t1 = opn.top();
                    }
                    if (!t1&&(op.top()->data.type)!=PLUSPLUS&&(op.top()->data.type)!=MINUSMINUS) {
                        error++;
                        break;
                    }
                    if(t1!=NULL)
                        opn.pop();
                    t = op.top();
                    if (!t) {
                        error++;
                        break;
                    }
                    op.pop();
                    t->l = t1;
                    t->r = t2;
                    opn.push(t);

                    p = (ASTTree *) malloc(sizeof(ASTTree));
                    p->data.data=NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(token_text1,token_text);
                    p->data.data=token_text1;
                    op.push(p);

                    w = gettoken(fp);
                    while(w==ANNO||w==INCLUDE){
                        w=gettoken(fp);
                    }
                    break;
                case '\0':
                    printf("第%d行出现警告\n",cnt_lines);
                    printf("警告：出现未知运算符\n");
                    exit(0);
            }
        }
            else{
            error=1;
        }
    };
    if((opn.size()==1)&&(op.top()->data.type==POUND)&&error==0){
        t=opn.top();
        opn.pop();
        root=(ASTTree*)malloc(sizeof(ASTTree));
        root->data.data=NULL;
        root->l=NULL;
        root->r=NULL;
        root->type=EXPRESSION;
        root->l=t;
        return root;
    } else{
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：表达式出现错误\n");
        exit(0);
    }
    //函数结束时w为endsym
}



/*比较优先级函数*/
char Precede(int c1,int c2){
    if(mistake==1){
        return NULL;
    }
    if(c1==PLUS||c1==MINUS){
        switch (c2){
            case PLUS:
            case MINUS:
            case RP:
            case POUND:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
                return '>';
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            default:
                return '\0';
                break;

        }
    } else if(c1==TIMES||c1==DIVIDE){
        switch (c2){
            case PLUS:
            case MINUS:
            case RP:
            case POUND:
            case TIMES:
            case DIVIDE:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
                return '>';
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            default:
                return '\0';
        }
    } else if (c1==LP){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
                return '=';
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if(c1==RP){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case PLUSPLUS:
            case MINUSMINUS:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if(c1==ASSIGN){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case POUND:
                return '>';
            default:
                return '\0';
        }
    }else if(c1==MORE||c1==LESS||c1==MOREEQUAL||c1==LESSEQUAL){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    }else if(c1==EQ||c1==NEQ){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    }else if(c1==POUND){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case RP:
            case EQ:
            case NEQ:
            case ASSIGN:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case POUND:
                return '=';
            default:
                return '\0';
        }
    }else if(c1==PLUSPLUS||c1==MINUSMINUS){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
            case POUND:
                return '>';
            case RP:
                return '<';
            default:
                return '\0';
        }
    }
}

void returntoken(int w,FILE* fp){
    int digit=strlen(token_text);
    int i;
    for(i=0;i<digit;i++){
        ungetc(token_text[digit-1-i],fp);
    }
}

void showType(int type){
    switch (type){
        case 1:
            printf("外部定义序列\n");
            break;
        case 2:
            printf("外部变量定义\n");
            break;
        case 3:
            printf("外部变量类型\n");
            break;
        case 4:
            printf("外部变量名序列\n");
            break;
        case 5:
            printf("外部变量名\n");
            break;
        case 6:
            printf("函数定义\n");
            break;
        case 7:
            printf("函数返回值类型\n");
            break;
        case 8:
            printf("函数名\n");
            break;
        case 9:
            printf("函数形参序列\n");
            break;
        case 10:
            printf("函数形参定义\n");
            break;
        case 11:
            printf("函数形参类型\n");
            break;
        case 12:
            printf("函数形参名\n");
            break;
        case 13:
            printf("函数体\n");
            break;
        case 14:
            printf("局部变量定义序列\n");
            break;
        case 15:
            printf("局部变量定义\n");
            break;
        case 16:
            printf("局部变量类型\n");
            break;
        case 17:
            printf("局部变量名序列\n");
            break;
        case 18:
            printf("局部变量名\n");
            break;
        case 19:
            printf("语句序列\n");
            break;
        case 20:
            printf("操作数\n");
            break;
        case 21:
            printf("运算符\n");
            break;
        case 22:
            printf("表达式\n");
            break;
        case 23:
            printf("if条件语句\n");
            break;
        case 24:
            printf("else语句体\n");
            break;
        case 25:
            printf("if语句\n");
            break;
        case 26:
            printf("if-else语句\n");
            break;
        case 27:
            printf("while语句\n");
            break;
        case 28:
            printf("while条件语句\n");
            break;
        case 29:
            printf("while语句体\n");
            break;
        case 30:
            printf("for语句\n");
            break;
        case 31:
            printf("for条件语句\n");
            break;
        case 32:
            printf("for语句条件一\n");
            break;
        case 33:
            printf("for语句条件二\n");
            break;
        case 34:
            printf("for语句条件三\n");
            break;
        case 35:
            printf("for语句体\n");
            break;
        case 36:
            printf("return语句\n");
            break;
        case 37:
            printf("break语句\n");
            break;
        case 38:
            printf("do-while语句\n");
            break;
        case 39:
            printf("do-while语句体\n");
            break;
        case 40:
            printf("do-while循环条件\n");
            break;
        case 41:
            printf("continue语句\n");
            break;
        case 42:
            printf("函数声明\n");
            break;
        case 43:
            printf("数组定义\n");
            break;
        case 44:
            printf("数组类型\n");
            break;
        case 45:
            printf("数组名\n");
            break;
        case 46:
            printf("数组大小\n");
            break;
    }
}

void PreorderTranverse(ASTTree* root,int depth){
    if(root== nullptr) printf("");
    else{
        int i;//控制缩进
        for(i=0;i<depth;i++){
            printf("\t");
        }
        showType(root->type);
        if(root->data.data!=NULL){
            for(i=0;i<depth;i++){
                printf("\t");
            }
            printf("%s\n",root->data.data);
        }
        PreorderTranverse(root->l,depth+1);
        PreorderTranverse(root->r,depth+1);
    }
}

int addname(char *token_text){
    if(mistake==1){
        return NULL;
    }
    int i,flag=0;
    VDN *p=Vroot;
    while(p->next!=NULL){
        p=p->next;
    }//新的变量名一定会被添加到最后一个结点中
    //添加变量之前先检查变量是否已经被定义过
    for(i=0;i<(p->size);i++){
        if(strcmp(token_text,p->variable[i])==0){
            flag=1;
            break;
        }
    }
    if(flag==1){
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：变量重复定义\n");
        exit(0);
        mistake=1;
        return flag;
    }
    char *savename=(char*)malloc(25* sizeof(char));
    strcpy(savename,token_text);
    p->variable[p->size]=savename;
    p->size++;
    return flag;

}

int checkname_exist(char* token_text){
    if(mistake==1){
        return NULL;
    }
    //返回值为1代表变量已经被定义过，为0代表变量没有被定义过
    //检查变量是否存在，只需要检查变量名是否在第一个结点或者最后一个结点
    int i;
    int flag=0;
    VDN* p=Vroot;
    while(p->next!=NULL){
        p=p->next;
    }
    for(i=0;i<(p->size);i++){
        if(strcmp(token_text,p->variable[i])==0){
            flag=1;
            break;
        }
    }
    for(i=0;i<(Vroot->size);i++){
        if(strcmp(token_text,Vroot->variable[i])==0){
            flag=1;
            break;
        }
    }
    if(flag==0){
        printf("第%d行出现错误\n",cnt_lines);
        printf("错误：使用了未被定义的变量\n");
        exit(0);
        mistake=1;
    }
    return flag;
}

