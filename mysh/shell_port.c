

//-----------------------------------------------------------------------
#include <string.h> //memcpy,memset
#include <stdio.h>  //EOF getchar putchar
#include <termio.h> //
//#include <stdarg.h>

#include "shell.h"
#include "printf.h"
#include "shell_port.h"

int sh_strlen(char *str)
{
    int i = 0;
    while(*str++ != '\0')
        i++;
    return i;
}

int sh_memset(void *p, int val, int size)
{
    memset(p, val, size);
    return 0;
}

int sh_memcpy(void *dst, void *src, int size)
{
    memcpy(dst, src, size);
    return 0;
}

//关于立即接收, https://www.cnblogs.com/life2refuel/p/5720043.html
//linux下的getchar默认是做了处理的,这里直接使用raw方式,应该就是嵌入式环境下的
//串口接收方式,嵌入式环境下uart使用while轮询方式recv,就是这里的sh_getch
//像 getchar 返回 EOF(ctrl+d),这些都是默认getchar才有的
int shell_getch(void)
{
    int cr;
    struct termios nts, ots;

    if (tcgetattr(0, &ots) < 0) //得到当前终端(0表示标准输入)的设置
        return EOF;

    nts = ots;
    cfmakeraw(&nts); //设置终端为Raw原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(0, TCSANOW, &nts) < 0) // 设置上更改之后的设置
        return EOF;

    cr = getchar();
    if (tcsetattr(0, TCSANOW, &ots) < 0) // 设置还原成老的模式
        return EOF;

    return cr;
}

int shell_putchar(char ch)
{
    putchar(ch);
    return 0;
}


//-----------------------------------------------------------------------

//我们只用提供 shell_getch 和 shell_putchar 的实现,然后调用 shell_printf_init
//然后就可以使用 shell_printf 了
// This function is required by printf function
void pputc ( void* p, char c) {
    shell_putchar(c);
}

int shell_printf_init()
{
    init_printf(0, pputc);
    return 0;
}

//int shell_printf(const char *restrict format, ...)
//{
//	va_list args;
//	va_start(args, format);
//	int ret = vprintf(format, args);
//	va_end(args);
//	return ret;
//}

//-----------------------------------------------------------------------

void help_usage() {
    shell_printf("usage: help <cmd-name>\r\n");
    shell_printf("<cmd-name> can be: \r\n");
    int i = 0;
    for (i = 0; i < cmd_tbl_items; i++) {
        shell_printf("%d.%s \r\n", i, cmd_tbl[i].cmd);
    }
}

cli_status_t help_func(int argc, char *argv[]) {
    int i = 0;
    //shell_printf("help_func execute, args are:\r\n");
    //for (i = 0; i < argc; i++) {
    //    shell_printf("arg%d: %s\r\n", i, argv[i]);
    //}
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    for (i = 0; i < cmd_tbl_items; i++) {
        if (match_cmd(cmd_tbl[i].cmd, argv[1])) {
            if (cmd_tbl[i].usage) {   //print usage
                (*cmd_tbl[i].usage)();
            }
            break;
        }
    }
    if (i == cmd_tbl_items) {
        shell_printf("error! not match any cmd.\r\n");
        return CLI_INVALID_ARGS;
    }
    return CLI_OK;
}

void echo_usage() {
    shell_printf("usage: echo <string>\r\n");
    shell_printf("\r\n");
}

cli_status_t echo_func(int argc, char *argv[]) {
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    shell_printf("%s\r\n", argv[1]);
    return CLI_OK;
}

//不支持负数
static int string_to_int(char *str) {
    char *a = str;
    int res = 0;
    while (*a) {
        if (*a > '0' && *a < '9') {
            res = res * 10 + (*a - '0');
        } else {
            return -1;
        }
        a++;
    }
    return res;
}

void history_usage() {
    shell_printf("usage: history <cmd-name>\r\n");
    shell_printf("<cmd-name> can be: \r\n");
    shell_printf("1.show\r\n2.clear\r\n3.<selected-history-cmd>\r\n");
}

cli_status_t history_func(int argc, char *argv[]) {
    int i = 0;
    int v = -1;
    int cmd_idx = 0;
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    if (match_cmd("show", argv[1])) {
        for(i = 1; i <= cmd_records.level; i++) {
            cmd_idx = (cmd_records.cmd_idx - i) % HISTORY_MAXITEMS; //计算在数组的真实index
            shell_printf("%d: %s\r\n", cmd_idx, cmd_records.cmd_buf[cmd_idx]);
        }
    } else if (match_cmd("clear", argv[1])) {
        cmd_records.level = 0;
    } else {
        v = string_to_int(argv[1]);
        if (v >= 0) {
            shell_printf("%s\r\n", cmd_records.cmd_buf[v]);
        }
    }
    return CLI_OK;
}

void exit_usage() {
    shell_printf("usage: exit shell\r\n");
}

cli_status_t exit_func(int argc, char *argv[]) {
    shell_printf("Bye.\r\n");
    shell_should_exit = 1;
    return CLI_OK;
}


