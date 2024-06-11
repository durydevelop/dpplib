#include "dpplib/DLog.h"
#include <cstdarg>
#include <cstdio>

namespace DTools
{
void DLog::info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%sINFO%s] ",CL_GREEN,CL_DEFAULT);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void DLog::debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%sDEBUG%s] ",CL_CYAN,CL_DEFAULT);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void DLog::error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%sERROR%s] ",CL_RED,CL_DEFAULT);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void DLog::warning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%sWARNING%s] ",CL_YELLOW,CL_DEFAULT);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
}