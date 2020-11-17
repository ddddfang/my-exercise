#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"


#define CMDBUFLEN 100


int run_cmd(char *cmd, ...)
{
    va_list ap;
    char buf[CMDBUFLEN];
    va_start(ap, cmd);
    vsnprintf(buf, CMDBUFLEN, cmd, ap);

    va_end(ap);

    DEBUG_PRINT("EXEC: %s\n", buf);
    return system(buf);
}


