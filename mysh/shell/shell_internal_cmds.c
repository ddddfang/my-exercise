

//-----------------------------------------------------------------------
#include "shell.h"
#include "shell_internal_cmds.h"
#include "common/printf.h"



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


