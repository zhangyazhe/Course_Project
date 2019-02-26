#include <stdio.h>
#include "gettoken.h"
#include <stdlib.h>
#include "word_analyse.h"
#include "syntax_analyse.h"
#include "format.h"

char filename[64];
FILE* fp;

int main(int argc,char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s <filename>", argv[0]);
        exit(-1);
    }
    int op;
    while (1) {
        system("clear");
        printf(" ——————————————————————\n");
        printf("     输入序号以选择功能    \n");
        printf("     1.词法分析          \n");
        printf("     2.语法分析          \n");
        printf("     3.格式化            \n");
        printf("     0.退出              \n");
        printf(" ——————————————————————\n");
        scanf("%d",&op);
        switch (op){
            case 1:
                fp=fopen(filename,"r");
                word_analyse();
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                getchar();
                break;
            case 2:
                fp=fopen(filename,"r");
                syntax_analyse();
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                getchar();
                break;
            case 3:
                fp=fopen(filename,"r");
                format();
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                getchar();
                break;
            case 0:
                system("clear");
                printf("感谢使用\n");
                return 0;
            default:
                printf("输入有误，请重新输入\n");
                printf("按任意键继续\n");
                getchar();
                getchar();
                break;
        }
    }
}