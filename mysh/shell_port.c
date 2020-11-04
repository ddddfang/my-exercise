#include "shell.h"


//-----------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <termio.h>

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

int shell_printf(const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);
	return ret;
}

//-----------------------------------------------------------------------

cli_status_t help_func(int argc, char *argv[]) {
    int i = 0;
    //shell_printf("help_func execute, args are:\n");
    //for (i = 0; i < argc; i++) {
    //    shell_printf("arg%d: %s\n", i, argv[i]);
    //}
    if (argc != 2) {
        shell_printf("usage: help <cmd-name>\n");
        shell_printf("<cmd-name> can be: ");
        for (i = 0; i < cmd_tbl_items; i++) {
            shell_printf("%s ", cmd_tbl[i].cmd);
        }
        shell_printf("\n");
        return CLI_INVALID_ARGS;
    }
    for (i = 0; i < cmd_tbl_items; i++) {
        if (match_cmd(cmd_tbl[i].cmd, argv[1])) {
            shell_printf("help %s not implement yet\n", argv[1]);
            break;
        }
    }
    if (i == cmd_tbl_items) {
        shell_printf("error! not match any cmd.\n");
        return CLI_INVALID_ARGS;
    }
    return CLI_OK;
}

cli_status_t history_func(int argc, char *argv[]) {
    int i = 0;
    shell_printf("history_func execute, args are:\n");
    for (i = 0; i < argc; i++) {
        shell_printf("arg%d: %s\n", i, argv[i]);
    }
    return CLI_OK;
}

cli_status_t exit_func(int argc, char *argv[]) {
    shell_printf("Bye.\n");
    shell_should_exit = 1;
    return CLI_OK;
}


