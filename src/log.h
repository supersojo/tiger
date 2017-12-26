#ifndef __LOG_H
#define __LOG_H
#include <stdarg.h>
#ifdef NDEBUG
# define Log(...)
#else
static int Log(const char* fmt,...){
    int cnt;
    va_list va;
    va_start(va,fmt);
    cnt = vprintf(fmt,va);
    va_end(va);
    return cnt;
}
#endif

#endif
