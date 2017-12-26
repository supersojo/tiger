#include <stdio.h>
#include "symbol.h"
/* interpret tiger ast */

typedef struct _expr_ret_t{
    enum{
        kIdExprRet,
        kNumExprRet,
        kStringExprRet
    }kind;
    union{
        char* name;
        int num;
        char* val;
    }u;
}expr_ret_t;

expr_ret_t interpreter_expr(expr_t* e);

expr_ret_t interpreter_prim(expr_prim_t* p)
{
    symbol_t* sym;
    expr_ret_t ret = {0};
    if(p->kind==kIdExprPrim) {
        sym = get_symbol(symtab,p->u.name);
        if(sym)
            ret.u.name = sym->name;
        ret.kind = kIdExprRet;
    }
    if(p->kind==kNumExprPrim) {
        ret.kind = kNumExprRet;
        ret.u.num = p->u.num;
    }
    if(p->kind==kStringExprPrim) {
        ret.kind = kStringExprRet;
    }
    return ret;
}
expr_ret_t interpreter_add_expr(expr_t* e1,expr_t* e2)
{
    int i=0,j=0;
    symbol_t* sym;
    expr_ret_t ret={0},ret1,ret2;
    ret1 = interpreter_expr(e1);
    ret2 = interpreter_expr(e2);
    if(ret1.kind==kIdExprRet) {
        sym = get_symbol(symtab,ret1.u.name);
        if(sym)
            i = sym->u.ival;
    }
    if(ret1.kind==kNumExprRet) {
        i = ret1.u.num;
    }
    if(ret2.kind==kIdExprRet) {
        sym = get_symbol(symtab,ret2.u.name);
        if(sym)
            j = sym->u.ival;
    }
    if(ret2.kind==kNumExprRet) {
        j = ret2.u.num;
    }
    ret.kind = kNumExprRet;
    ret.u.num = i+j;
    return ret;
}
expr_ret_t interpreter_expr(expr_t* e)
{
    expr_ret_t ret = {0};
    if(e->kind==kPrimExpr) {
        ret = interpreter_prim(e->u.prim);
    }
    if(e->kind==kAddExpr) {
        ret = interpreter_add_expr(e->u.opers.expr1,e->u.opers.expr2);
    }
    return ret;
}

void interpreter_assign_stat(assign_stat_t* t)
{
    int i = 0;
    symbol_t* sym;
    expr_ret_t ret1,ret2;
    expr_t *le,
           *re;
           
    le = t->lexp;
    re = t->rexp;
    
    ret1 = interpreter_expr(le);
    ret2 = interpreter_expr(re);
    if(ret1.kind!=kIdExprRet) {
        printf("wrong type\n");
    }
    if(ret2.kind==kIdExprRet) {
        sym = get_symbol(symtab,ret2.u.name);
        if(sym)
            i = sym->u.ival;
    }
    if(ret2.kind==kNumExprRet) {
        i = ret2.u.num;
    }
    sym = get_symbol(symtab,ret1.u.name);
    if(sym)
        sym->u.ival = i;
    
}
void interpreter_print_stat(print_stat_t* t)
{
    symbol_t* sym;
    int i;
    expr_ret_t ret;
    ret = interpreter_expr(t->exp);
    if(ret.kind==kIdExprRet) {
        sym = get_symbol(symtab,ret.u.name);
        if(sym)
            i = sym->u.ival;
    }
    if(ret.kind==kNumExprRet) {
        i = ret.u.num;
    }
    printf("%d\n",i);
    
}
void interpreter_stat(stat_t* t)
{
    if(t->kind==kAssignStat) {
        interpreter_assign_stat(t->u.assign_stat);
    }
    if(t->kind==kPrintStat) {
        interpreter_print_stat(t->u.print_stat);
    }
}

void interpreter(prog_t* p)
{
    stat_t* t = 0;
    t = p->stat_list;
    while(t){
        interpreter_stat(t);
        t = t->next;
    }
}