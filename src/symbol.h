#ifndef SYMBOL_H
#define SYMBOL_H

/* sematics */
typedef struct _expr_prim_t{
    enum{
        kIdExprPrim,
        kNumExprPrim,
        kStringExprPrim,
        kVoidExprPrim
    }kind;
    union{
        char* name;
        int num;
        char* val;
    }u;
}expr_prim_t;

typedef struct _expr_t{
        enum{
            kPrimExpr,
            kAddExpr,
            kSubExpr,
            kMulExpr,
            kDivExpr
        }kind;
        union{
            expr_prim_t* prim;
            enum{
                kAddOp,
                kSubOp,
                kMulOp,
                kDivOp
            }op;
            /* for add/sub/mul/div */
            struct{
                struct _expr_t *expr1,
                               *expr2;
            }opers;
        }u;
        int type;/* kUnUsedSymType or kIntSymType or kStrSymType */
        
        /* code generation */
        /* prim is the id */
        /* others a new symbol is binded */

        char* binding;
        
}expr_t;

typedef struct _assign_stat_t{
    expr_t* lexp;
    expr_t* rexp;
}assign_stat_t;

typedef struct _print_stat_t{
    expr_t* exp;
}print_stat_t;

typedef struct _stat_t{
    enum{
        kAssignStat,
        kPrintStat,
    }kind;
    union{
        assign_stat_t* assign_stat;
        print_stat_t* print_stat;
    }u;
    struct _stat_t* next;
}stat_t;

typedef struct _prog_t{
    stat_t* stat_list;
}prog_t;

expr_prim_t* new_expr_prim();

expr_t* new_expr();

assign_stat_t* new_assign_stat();

print_stat_t* new_print_stat();

stat_t* new_stat();

prog_t* new_prog();

/* symbol table */
typedef struct _symbol_t{
    enum{
        kUnUsedSymType,
        kIntSymType,
        kStrSymType
    }type;
    union{
        int ival;
        char* sval;
    }u;
    char* name;
    struct _symbol_t* next;
}symbol_t;

symbol_t* new_symbol();

typedef struct _symbol_table_t{
    int (*hash)(char* key);
    symbol_t* syms[16];
    int level;
    struct _symbol_table_t* next;
}symbol_table_t;

symbol_table_t* new_symbol_table(int (*hash)(char* name));

symbol_t* get_symbol(symbol_table_t* t,char* name);

int add_symbol(symbol_table_t* t,char* name);

int add_int_symbol(symbol_table_t* t,char* name,int type,int val);

int add_str_symbol(symbol_table_t* t,char* name,int type,char* val);

/* type check*/
int check_expr_type(expr_t* e1,expr_t* e2);

/* xxx001 xxx002*/
char* new_tmp(char* tag);

/* code generation */
typedef struct _code_t{
    char* data_seg;
    char* code_seg;
}code_t;
code_t* new_code();

void gen_data(code_t* code,char* ins);
void gen_code(code_t* code,char* ins);


/* global variables */
extern prog_t* prog_head;
extern symbol_table_t* symtab;
extern code_t* code;

void free_ast(prog_t* prog);
void free_symbol_table(symbol_table_t* t);

#endif
