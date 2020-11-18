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


static uint32_t sum_every_16bits(void *addr, int count)
{
    register uint32_t sum = 0;
    uint16_t * ptr = addr;

    while( count > 1 )  {
        /*  This is the inner loop */
        sum += * ptr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += * (uint8_t *) ptr;

    return sum;
}

//ip 首部校验和,对 20 Bytes 的 ip 首部(初始时首部校验和字段应该是0x0000)应用此算法
//将计算结果填充到首部校验和字段.校验的时候再次应用此算法,若结果为0说明数据应该是good
uint16_t checksum(void *addr, int count, int start_sum)
{
    /* Compute Internet Checksum for "count" bytes
    *         beginning at location "addr".
    * Taken from https://tools.ietf.org/html/rfc1071
    */
    uint32_t sum = start_sum;

    sum += sum_every_16bits(addr, count);

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

