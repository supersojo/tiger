/*
识别词法
标识符
整数
字符串常亮
print关键字
=操作符
左右括号
由于开始设计动态类型，但是后端要生成汇编代码，所以仅支持全局作用域，赋值语句，print函数，算符优先级未处理
*/

/* c 变量声明 */
%{
#include <stdarg.h>
#include <stdio.h>
#include "token.h"

#include "log.h"

%}

/* 标记声明 */
DIGIT [0-9]
LETTER [A-Za-z]

/* 
状态声明 
lex至少有一个状态INITIAL，用户可以定义更多的状态，通过BEGIN执行状态装换
*/

/*--------------------------------------------------------------------*/
/* 规则部分中注释行首要有前导空白 */
%%
print {
    Log("keyword %s is detected\n",yytext);
    return kTigerTokenTypePRINT;
}

 /* id */
([_]|{LETTER})([_]|{LETTER}|{DIGIT})* {
    Log("id %s is detected\n",yytext);
    yylval.name = strdup(yytext);
    return kTigerTokenTypeID;
}

 /* number */
[0]|{DIGIT}+   {
    Log("number %s is detected\n",yytext);
    yylval.num = atoi(yytext);
    return kTigerTokenTypeNUM;
}

 /* const string */
[\"](\\.|[^\\"])*[\"] {
    char* p = strdup(yytext);
    /* strip \" */
    *(p+strlen(p)-1)='\0';
    yylval.val = strdup(p+1);
    free(p);
    //yylval.val = yytext;
    Log("const string %s is detected\n",yylval.val);
    return kTigerTokenTypeSTRING;
}

[=] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeEQUAL;
}

[+] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeADD;
}

[-] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeSUB;
}

[\*] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeMUL;
}

[/] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeDIV;
}

[(] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeLP;
}

[)] {
    Log("operator %s is detected\n",yytext);
    return kTigerTokenTypeRP;
}

[\n] {}

. {}
  

%%

/* c 代码 */
int yywrap()
{
    return 1;
}