//
//  stream.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/16.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "stream.h"

uint8_t *file_buff;

#ifdef TRACE
FILE *trace_fp;
#endif


// 读取h264文件，读取失败返回-1，否则返回文件大小
int readAnnexbBitStreamFile(const char *fp)
{
    FILE *fp_h264 = fopen(fp, "rb");
    if (fp_h264 == NULL) {
        printf("打开h264文件失败\n");
        return -1;
    }
    
    file_buff = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    int file_size = (int)fread(file_buff, sizeof(uint8_t), MAX_BUFFER_SIZE, fp_h264);
    fclose(fp_h264);

#if TRACE
    trace_fp = fopen("trace_dec.txt", "w");
    if (trace_fp == NULL) {
        printf("打开trace_dec.txt文件失败\n");
        return -1;
    }
#endif
    return file_size;
}

void freeFilebuffer(void)
{
    free(file_buff);
#if TRACE
    fclose(trace_fp);
#endif
}


#if TRACE
void traceInput(const char *traceString, uint32_t eleValue)
{
    int inputCharsCount = 0;

    putc('@', trace_fp);

    // 1.录入traceString
    inputCharsCount += fprintf(trace_fp, " %s", traceString);
    while(inputCharsCount++ < 55) { //做一个align
        putc(' ',trace_fp);
    }

    // 2.录入eleValue
    fprintf(trace_fp, "  (%3d)\n", eleValue);

    // 3.将缓冲区的内容输出到文件中
    fflush(trace_fp);
}

#include <stdarg.h>
#include <string.h>
void tracePrintf(const char *fmt, ...)
{
    va_list marker;
    char buffer[512];
    int nLen;

    va_start(marker, fmt);
    vsnprintf(buffer, 512, fmt, marker);
    va_end(marker);

    nLen = strlen(buffer);

    if(buffer[nLen -1] != '\n')
    {
        buffer[nLen] = '\n';
        buffer[nLen + 1] = '\0';
    }

    if(trace_fp)
    {
        fprintf(trace_fp, "%s", buffer);
        fflush(trace_fp);
    }
}

#endif


