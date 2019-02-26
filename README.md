# 高级语言源程序语法分析与格式处理工具
## 功能介绍
### 1.词法分析
该功能可以识别源代码中的词语单元（例如标识符、符号等）并分析出词语单元在程序中的成分，最终显示在屏幕上
### 2.语法分析
该功能可以检测源代码中的语法错误并报错；如果代码语法正确，则会生成程序的语法树并将语法树输出。
### 3.代码格式化
该功能可以控制缩进从而将代码标准化代码的格式
## 测试
您可以使用以下测试样例
```
#include <stdio.h>
int _a;
long b;
char _d,_e;
double c,h;
int num[10];
int i;
int func(int a,int b){
a=1;
b=2l;
c=.14;
h=3.13f;
_d='\x22';
_e='\t';
a=1+b;
b=2+b;
b=c;
if(a==b)
a=a+b;
else if(a<b){
a=a-b;
num[10]="string";
if(a>b){
a=a+1;
}
}else{
a=a/b;
a=a+1;
}
if(a>b)
if(a>b)
a++;
if(a>b){
a=(a+1)*3;
b=b+c+2;
}
do
{
a++;
}while(a>b);
while(1){
if(a>b){
a=(a+1)*3;
b=b+c+2;
break;
}
if(a<b){
continue;
}
while(a==1){
a++;
}
}
for(i=0;i<a;i++){
a=a+1;
}
for(;;i--){
a=(a+1)*2;
}
do{
a=b+c;
}while(a>1);
return a+b;
}
/*函数声明*/
void def();

~
```

注意：本程序只能在命令行中执行，执行时需要两个参数，第一个是可执行文件的文件名，第二个是测试样例文件的相对路径。
例如
```
$ ./lanuage_format_processing sample1.txt 
```

经测试，本程序可以在linux/unix系统终端中完美运行。
