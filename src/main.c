#include <stdio.h>
#include "symbol.h"
extern FILE* yyin;
extern int yyparse();

extern interpreter(prog_t* p);

char asm[64*1024] = {0};
int asm_len = 0;

void gen_asm()
{
    int pos = 0;
    pos += sprintf(asm+pos,".data\n");
    pos += sprintf(asm+pos,"%s",code->data_seg);
    pos += sprintf(asm+pos,".code\n");
    pos += sprintf(asm+pos,"extern print:PROC\n");
    pos += sprintf(asm+pos,"main PROC\n");
    pos += sprintf(asm+pos,"%s",code->code_seg);
    pos += sprintf(asm+pos,"ret\n");
    pos += sprintf(asm+pos,"main ENDP\n");
    pos += sprintf(asm+pos,"END\n");
    asm_len = pos;
}
void write_file()
{
    FILE* f = 0;
    f = fopen("./src/t.asm","w");
    fwrite(asm,asm_len,1,f);
    fclose(f);
}
void print(int n)
{
    printf("%d",n);
}

int main()
{
    int ret = 0;
    yyin = fopen("test.tiger","r");
    if(yyin==0)
        printf("open test.tiger error\n");
    
    /* syntax analysis */
    ret = yyparse();
    if(ret)
    {
        printf("parse error\n");
    }
    else
    {
        /* gen asm file in src/ */
        gen_asm();
        write_file();
    }
    
    /* interpreter */
    interpreter(prog_head);
    
    /* free resources */
    free_ast(prog_head);
    free_symbol_table(symtab);
    free(symtab);
    free_code(code);
    free(code);
    
    return 0;
}