#include "shell.h"
#include "funcs.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>


//--------------------------------------------------
int shell_should_exit = 0;
//所有支持的命令
cmd_t cmd_tbl[] = {
    {
        .cmd = "help",
        .func = help_func
    },
    {
        .cmd = "history",
        .func = history_func
    },
    {
        .cmd = "exit",
        .func = exit_func
    }
};
int cmd_tbl_items = 0;
//命令执行记录
cmd_records_t cmd_records;
//--------------------------------------------------



char *status_to_str(cli_status_t st)
{
    switch (st) {
        case CLI_CMD_NOT_FOUND:
            return "CLI_CMD_NOT_FOUND";
        case CLI_INVALID_ARGS:
            return "CLI_INVALID_ARGS";
        case CLI_EXE_ERROR:
            return "CLI_EXE_ERROR";
        default:
            return NULL;
    }
    return NULL;
}



int shell_printf(const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);
	return ret;
}

//关于立即接收,(我还没看) https://www.cnblogs.com/life2refuel/p/5720043.html
int read_input(char *input)
{
	int i = 0;
	char c;

	//while ((c = fgetc(stdin)) != '\n') {
    //while (read(STDIN_FILENO, &c, 1) > 0 && c != '\n') {
	//while ((c = getc(stdin)) != '\n') {
	while ((c = getchar()) != '\n') {
		/* did user enter ctrl+d ?*/
		if (c == EOF) {
            DEBUG_PRINT("info! EOF pressed!\n");
			return -1;
		}
        if (c == '\t') {
            DEBUG_PRINT("info! tab pressed!\n");
        }
		/* allocate more memory for input */
		if (i >= COMMAND_BUF_SIZE) {
            DEBUG_PRINT("error! input exceed COMMAND_BUF_SIZE(%d)\n", COMMAND_BUF_SIZE);
			return -1;
		}
		input[i++] = c;
	}
	input[i] = '\0';
	return 0;
}

int is_blank(char *input)
{
	int n = (int) strlen(input);
	int i;

	for (i = 0; i < n; i++) {
		if (input[i] != ' ')
			return 0;
	}
	return 1;
}

int parse_command(char *input, cmd_parsed_t *output)
{
	int n = (int) strlen(input);
	int i, oi = 0;
    output->argc = 0;
	for (i = 0; i < n; i++) {
		if (input[i] != ' ' && input[i] != '\t') {
            if (oi >= COMMAND_BUF_SIZE) {
                DEBUG_PRINT("error: args too long!\n");
                return -1;
            }
            if (output->argc >= ARG_MAX_COUNT) {
                DEBUG_PRINT("error: too many args !\n");
                return -1;
            }
            output->args[output->argc][oi++] = input[i];
            if (i == n-1) {
                output->args[output->argc][oi] = '\0';
                output->argc++;
            }
        } else {
            if (oi != 0) {  //说明不是连续的空格
                output->args[output->argc][oi] = '\0';
                output->argc++;
                oi = 0;
            }
        }
	}
    for (i = 0; i < output->argc; i++) {
        output->argv[i] = output->args[i];
    }
    output->argv[output->argc] = 0;
	return 0;
}

int match_cmd(char *a, char *b)
{
	int n = (int) strlen(a);
	int i;
    if (n != (int)strlen(b)) {
        return 0;
    }
	for (i = 0; i < n; i++) {
		if (a[i] != b[i])
			return 0;
	}
	return 1;

}

cli_status_t execute_command(cmd_parsed_t *cmd)
{
    int i = 0;
    //int len = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);
    int len = cmd_tbl_items;

    for (i = 0; i < len; i++) {
        if (match_cmd(cmd_tbl[i].cmd, cmd->argv[0])) {
            return (*cmd_tbl[i].func)(cmd->argc, cmd->argv);
        }
    }
    return CLI_CMD_NOT_FOUND;
}

int main() {
    cmd_tbl_items = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);
    char cur_cmd_buf[COMMAND_BUF_SIZE] = {0};
    cli_status_t rc = CLI_OK;

    while (shell_should_exit == 0) {
        shell_printf(">>");
        if (read_input(cur_cmd_buf) != 0) {
            DEBUG_PRINT("error! read_input error\n");
            continue;
        }
        if (strlen(cur_cmd_buf) > 0 && !is_blank(cur_cmd_buf)) {
            cmd_parsed_t cmd_parsed = {0};

            if (parse_command(cur_cmd_buf, &cmd_parsed) != 0) {
                DEBUG_PRINT("error! parse_command error\n");
                continue;
            }

            //add to history
            memcpy(cmd_records.cmd_buf[cmd_records.cmd_idx], cur_cmd_buf, sizeof(cur_cmd_buf));
            cmd_records.cmd_idx = (cmd_records.cmd_idx + 1) % HISTORY_MAXITEMS;

            if ((rc = execute_command(&cmd_parsed)) != CLI_OK) {
                shell_printf("%s\n", status_to_str(rc));
            }
        }
    }
    return 0;
}

