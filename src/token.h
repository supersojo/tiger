#ifndef __TOKEN_H
#define __TOKEN_H

#include "symbol.h"
#include "tiger.tab.h"


/*
id
num

*/
#define MAKE_TOKEN_TYPE(type) kTigerTokenType##type
#define MAKE_TOKEN_TYPE_STRING(type,str) #str

typedef enum
{
    kTigerTokenTypeUNUSED=0,
    MAKE_TOKEN_TYPE(ID)=id,
    MAKE_TOKEN_TYPE(NUM)=num,
    MAKE_TOKEN_TYPE(STRING)=string,
    MAKE_TOKEN_TYPE(EQUAL)=equal,
    MAKE_TOKEN_TYPE(ADD)=add,
    MAKE_TOKEN_TYPE(SUB)=sub,
    MAKE_TOKEN_TYPE(MUL)=mul,
    MAKE_TOKEN_TYPE(DIV)=div1,
    MAKE_TOKEN_TYPE(PRINT)=print,
    MAKE_TOKEN_TYPE(LP)=lp,
    MAKE_TOKEN_TYPE(RP)=rp,
}TigerTokenType;


#endif
