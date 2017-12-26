/*
识别句型
识别赋值语句、print语句
全局符号表
*/

/* prologue */
%{
#include "log.h"
#include "token.h"
#include "symbol.h"

/* global variables */

%}

/* bison declarations*/
%union{
char* name;
int num;
char* val;
expr_prim_t* expr_prim;
expr_t* expr;
assign_stat_t* assign_stat;
print_stat_t* print_stat;
stat_t* stat;
prog_t* prog;
}

/* terminal symbol */
%token <name> id
%token <num> num
%token <val> string
%token equal
%token add
%token sub
%token mul
%token div1
%token print
%token lp
%token rp

/* non-terminal symbol */
%type <prog> prog
%type <stat> stat
%type <expr> expr
%type <expr_prim> expr_prim
%type <assign_stat> assign_stat
%type <print_stat> print_stat

%%

/* Grammar rules */
prog:
/*empty*/ {
    prog_t* p = new_prog();
    p->stat_list = 0;
    $$ = p;
    prog_head = p;
    
    /* init symbol table */
    symtab = new_symbol_table(0);
    
    /* code generator */
    code = new_code();
    
}
|prog stat {
    prog_t* p = $1;
    stat_t* t = $2;
    stat_t* tmp = 0;
    /* add stat to prog's list */
    if(p->stat_list){
        tmp = p->stat_list;
        while(tmp->next){
            tmp = tmp->next;
        }
        tmp->next = t;
    }else
    {
        p->stat_list = t;
    }
    $$ = $1;
}
;



stat:
assign_stat {
    stat_t* t = new_stat();
    t->kind = kAssignStat;
    t->u.assign_stat = $1;
    $$ = t;
}
| print_stat {
    stat_t* t = new_stat();
    t->kind = kPrintStat;
    t->u.print_stat = $1;
    $$ = t;
}
;

assign_stat:
expr equal expr {
    char ins[1024] = {0};
    symbol_t* sym = 0;
    assign_stat_t* t = new_assign_stat();
    t->lexp = $1;
    t->rexp = $3;
    $$ = t;
    Log("lexp type %d equal rexp type %d\n",$1->type,$3->type);
    if(t->lexp->kind!=kPrimExpr) {
        Log("lexp must be prim expr\n");
        return 1;
    }
    if(t->lexp->u.prim->kind!=kIdExprPrim) {
        Log("lexp must be id expr prim\n");
        return 1;
    }
    
    sym = get_symbol(symtab,t->lexp->u.prim->u.name);
    if(sym==0) {
        Log("get symbol %s error\n",t->lexp->u.prim->u.name);
        return 1;
    }
    
    sym->type = $3->type;
    
    /* assign will overwrite lexp's type */
    t->lexp->type = t->rexp->type;
    
    /* code generation */
    if($1->type==kIntSymType) {
        sprintf(ins,"mov %s,%s","eax",$3->binding);
        gen_code(code,ins);
        sprintf(ins,"mov %s,%s",$1->binding,"eax");
        gen_code(code,ins);
    }
}
;

print_stat:
print lp expr rp {
    char ins[1024]={0};
    print_stat_t* p = 0;
    p = new_print_stat();
    p->exp = $3;
    /* code generation */
    if($3->type==kIntSymType) {
        sprintf(ins,"sub rsp,28h\n"
                     "xor rcx,rcx\n"
                     "mov ecx,%s",$3->binding);
        gen_code(code,ins);
        sprintf(ins,"call print");
        gen_code(code,ins);
        sprintf(ins,"add rsp,28h");
        gen_code(code,ins);
    }
    $$ = p;
}
;

expr:
expr_prim {
    char ins[1024]={0};
    symbol_t* sym = 0;
    expr_t*e = new_expr();
    e->kind = kPrimExpr;
    e->u.prim = $1;
    $$ = e;
    if(e->u.prim->kind==kIdExprPrim) {
        sym = get_symbol(symtab,e->u.prim->u.name);
        if(sym)
            e->type = sym->type;
    }
    if(e->u.prim->kind==kNumExprPrim)
        e->type = kIntSymType;
    if(e->u.prim->kind==kStringExprPrim)
        e->type = kStrSymType;
        
    /* binding */
    if(e->u.prim->kind==kIdExprPrim) {
        e->binding = (char*)strdup(e->u.prim->u.name);
    }
    if(e->u.prim->kind==kNumExprPrim) {
        e->binding = (char*)strdup(new_tmp("INT"));
        
        sprintf(ins,"%s dword %d",e->binding,e->u.prim->u.num);
        gen_data(code,ins);
    }
    if(e->u.prim->kind==kStringExprPrim) {
        e->binding = (char*)strdup(new_tmp("STR"));
        sprintf(ins,"%s dword %s_addr",e->binding,e->binding);
        gen_data(code,ins);
        sprintf(ins,"%s_addr db '%s',0",e->binding,e->u.prim->u.val);
        gen_data(code,ins);
    }
}
| expr add expr {
    char ins[1024] = {0};
    expr_t*e = new_expr();
    e->kind = kAddExpr;
    Log("add expr with kind [%d,%d]\n",$1->kind,$3->kind);
    e->u.op = kAddOp;// maybe unused
    e->u.opers.expr1 = $1;
    e->u.opers.expr2 = $3;
    $$ = e;
    /* type check */
    Log("e1 type %d add e2 type %d\n",$1->type,$3->type);
    if(!check_expr_type($1,$3)) {
        Log("type error\n");
        return 1;
    }
    $$->type = $1->type;
    
    /* code generation */
    if($$->type==kIntSymType) {
        e->binding = (char*)strdup(new_tmp("INT"));
        sprintf(ins,"%s dword ?",e->binding);
        gen_data(code,ins);
        sprintf(ins,"mov %s,%s","eax",$1->binding);
        gen_code(code,ins);
        sprintf(ins,"add %s,%s","eax",$3->binding);
        gen_code(code,ins);
        sprintf(ins,"mov %s,%s",e->binding,"eax");
        gen_code(code,ins);
    }
    if($$->type==kStrSymType) {
        e->binding = (char*)strdup(new_tmp("STR"));
        gen_code(code,ins);
    }
    
}
| expr sub expr {
    char ins[1024] = {0};
    expr_t*e = new_expr();
    e->kind = kSubExpr;
    Log("sub expr with kind [%d,%d]\n",$1->kind,$3->kind);
    e->u.op = kSubOp;// maybe unused
    e->u.opers.expr1 = $1;
    e->u.opers.expr2 = $3;
    $$ = e;
    /* type check */
    Log("e1 type %d add e2 type %d\n",$1->type,$3->type);
    if(!check_expr_type($1,$3)) {
        Log("type error\n");
    }
    $$->type = $1->type;
    
    /* code generation */
    if($$->type==kIntSymType) {
        e->binding = (char*)strdup(new_tmp("INT"));
        sprintf(ins,"%s dword ?",e->binding);
        gen_data(code,ins);
        sprintf(ins,"mov %s,%s","eax",$1->binding);
        gen_code(code,ins);
        sprintf(ins,"sub %s,%s","eax",$3->binding);
        gen_code(code,ins);
        sprintf(ins,"mov %s,%s",e->binding,"eax");
        gen_code(code,ins);
    }
    if($$->type==kStrSymType) {
        e->binding = (char*)strdup(new_tmp("STR"));
        gen_code(code,ins);
    }
}
| expr mul expr {
    /* not implement */
    expr_t*e = new_expr();
    e->kind = kMulExpr;
    e->u.op = kMulOp;// maybe unused
    e->u.opers.expr1 = $1;
    e->u.opers.expr2 = $3;
    $$ = e;
}
| expr div1 expr {
    /* not implement */
    expr_t*e = new_expr();
    e->kind = kDivExpr;
    e->u.op = kDivOp;// maybe unused
    e->u.opers.expr1 = $1;
    e->u.opers.expr2 = $3;
    $$ = e;
}
;

expr_prim:
/*empty*/ {
    /* void expr for print() */
    expr_prim_t* prim = new_expr_prim();
    prim->kind = kVoidExprPrim;
    $$ = prim;
}
| id {
    char ins[1024]={0};
    int ret = 0;
    symbol_t* sym = 0;
    expr_prim_t* prim = new_expr_prim();
    prim->kind = kIdExprPrim;
    prim->u.name = (char*)strdup($1);
    $$ = prim;
    
    /* add to symbol table */
    sym = get_symbol(symtab,$1);
    if(sym==0) {
        ret = add_symbol(symtab,$1);
        if(ret)
            Log("add_symbol %s error\n",$1);
        
        sprintf(ins,"%s dword ?",$1);
        gen_data(code,ins);
    }
}
| num {
    expr_prim_t* prim = new_expr_prim();
    prim->kind = kNumExprPrim;
    Log("num prim %d\n",$1);
    prim->u.num = $1;
    $$ = prim;
}
| string{
    expr_prim_t* prim = new_expr_prim();
    prim->kind = kStringExprPrim;
    prim->u.val = (char*)strdup($1);
    $$ = prim;
}
;



%%

/* Epilogue */
int yyerror(char* msg)
{
    printf("tiger error:%s\n",msg);
    return 1;
}
