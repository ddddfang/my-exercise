#include "shell.h"


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
            //
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


