#include "symbol.h"

#ifndef NULL
#define NULL 0
#endif

/* global variables */
prog_t* prog_head = 0;
symbol_table_t* symtab = 0;
code_t* code = 0;

/* functions for sematics (AST) */
expr_prim_t* new_expr_prim()
{
    expr_prim_t* p = NULL;
    p = (expr_prim_t*)malloc(sizeof(expr_prim_t));
    memset(p,0,sizeof(expr_prim_t));
    return p;
}
expr_t* new_expr()
{
    expr_t* p = NULL;
    p = (expr_t*)malloc(sizeof(expr_t));
    memset(p,0,sizeof(expr_t));
    return p;
}
assign_stat_t* new_assign_stat()
{
    assign_stat_t* p = NULL;
    p = (assign_stat_t*)malloc(sizeof(assign_stat_t));
    memset(p,0,sizeof(assign_stat_t));
    return p;
}
print_stat_t* new_print_stat()
{
    print_stat_t* p = NULL;
    p = (print_stat_t*)malloc(sizeof(print_stat_t));
    memset(p,0,sizeof(print_stat_t));
    return p;
}
stat_t* new_stat()
{
    stat_t* p = NULL;
    p = (stat_t*)malloc(sizeof(stat_t));
    memset(p,0,sizeof(stat_t));
    return p;
}
prog_t* new_prog()
{
    prog_t* p = NULL;
    p = (prog_t*)malloc(sizeof(prog_t));
    memset(p,0,sizeof(prog_t));
    return p;
}

symbol_t* new_symbol()
{
    symbol_t* p = NULL;
    p = (symbol_t*)malloc(sizeof(symbol_t));
    memset(p,0,sizeof(symbol_t));
    return p;
}
static int default_hash(char* key)
{
    int n = 0;
    int i = 0;
    int s = strlen(key);
    
    for(i = 0;i<s;i++)
        n += key[i];
    
    n%=16;
    
    return n;
}
symbol_table_t* new_symbol_table(int (*hash)(char* name))
{
    symbol_table_t* p = NULL;
    p = (symbol_table_t*)malloc(sizeof(symbol_table_t));
    memset(p,0,sizeof(symbol_table_t));
    
    if(hash==0)
    {
        p->hash = default_hash;
    }
    
    return p;
}

symbol_t* get_symbol(symbol_table_t* t,char* name)
{
    int i = 0;
    symbol_t* sym = 0;
    i = t->hash(name);
    sym = t->syms[i];
    while(sym)
    {
        if(strcmp(sym->name,name)==0)
            break;
        sym = sym->next;
    }
    
    return sym;
}

int add_symbol(symbol_table_t* t,char* name)
{
    symbol_t* sym = 0;
    symbol_t* n = 0;
    int i = 0;
    
    n = new_symbol();
    
    n->name = strdup(name);
        
    i = t->hash(name);
    sym = t->syms[i];
    if(sym==0)
    {

        
        t->syms[i] = n;
    }
    else
    {
        while(sym->next)
        {
            sym = sym->next;
        }
        sym->next = n;
    }
    return 0;
}

int add_int_symbol(symbol_table_t* t,char* name,int type,int val)
{
    int ret = 0;
    symbol_t* sym = 0;
    ret = add_symbol(t,name);
    if(ret==0){
        sym = get_symbol(t,name);
        if(sym)
            sym->u.ival = val;
    }
    return 0;
}


int add_str_symbol(symbol_table_t* t,char* name,int type,char* val)
{
    int ret = 0;
    symbol_t* sym = 0;
    ret = add_symbol(t,name);
    if(ret==0){
        sym = get_symbol(t,name);
        if(sym)
            sym->u.sval = strdup(val);
    }
    return 0;
}

int check_expr_type(expr_t* e1,expr_t* e2)
{
    return (e1->type==e2->type);
}

char* new_tmp(char* tag)
{
    static int i=0;
    static char tmp[64];
    memset(tmp,0,64);
    sprintf(tmp,"%s%02d",tag,i);
    i++;
    return tmp;
}


code_t* new_code()
{
    code_t* p = NULL;
    p = (code_t*)malloc(sizeof(code_t));
    memset(p,0,sizeof(code_t));
    p->data_seg = malloc(10240);
    p->code_seg = malloc(10240);
    memset(p->data_seg,0,10240);
    memset(p->code_seg,0,10240);
    return p;
}

void free_code(code_t* code)
{
    free(code->data_seg);
    free(code->code_seg);
}

void gen_code(code_t* code,char* ins)
{
    sprintf(code->code_seg+strlen(code->code_seg),"%s\n",ins);
}

void gen_data(code_t* code,char* ins)
{
    sprintf(code->data_seg+strlen(code->data_seg),"%s\n",ins);
}

void free_expr_prim(expr_prim_t* prim)
{
    if(prim->kind==kIdExprPrim) {
        free(prim->u.name);
    }
    if(prim->kind==kStringExprPrim) {
        free(prim->u.val);
    }
    if(prim->kind==kNumExprPrim) {
        ;
    }
}
void free_expr(expr_t* e)
{
    if(e->kind==kPrimExpr) {
        free_expr_prim(e->u.prim);
        free(e->u.prim);
    }
    if(e->kind==kAddExpr||e->kind==kSubExpr||e->kind==kMulExpr||e->kind==kDivExpr) {
        free_expr(e->u.opers.expr1);
        free_expr(e->u.opers.expr2);
        free(e->u.opers.expr1);
        free(e->u.opers.expr2);
        
        if(e->binding)
            free(e->binding);
    }
}
void free_assign_stat(assign_stat_t* t)
{
    free_expr(t->lexp);
    free_expr(t->rexp);
}
void free_print_stat(print_stat_t* t)
{
    free_expr(t->exp);
    free(t->exp);
}
void free_stat(stat_t* t)
{
    if(t->kind==kAssignStat) {
        free_assign_stat(t->u.assign_stat);
        free(t->u.assign_stat);
    }
    if(t->kind==kPrintStat) {
        free_print_stat(t->u.print_stat);
        free(t->u.print_stat);
    }
}
void free_ast(prog_t* prog)
{
    int i = 0;
    stat_t* p = prog->stat_list;
    while(p) {
        prog->stat_list = p->next;
        free_stat(p);
        free(p);
        i++;
        p = prog->stat_list;
    }
}
void free_symbol(symbol_t* sym)
{
    free(sym->name);
}
void free_symbol_table(symbol_table_t* t)
{   
    int i = 0;
    symbol_t* sym;
    
    for(i = 0; i<16; i++) {
        sym = t->syms[i];
        while(sym) {
            t->syms[i] = sym->next;
            free_symbol(sym);
            free(sym);
            sym = sym->next;
        }
    }
}

