#include "shell.h"
#include "shell_port.h"

#include <string.h> //memcpy

//ascii
// [up] key: 0x1b 0x5b 0x41
// [down] key: 0x1b 0x5b 0x42
// [right] key: 0x1b 0x5b 0x43
// [left] key: 0x1b 0x5b 0x44
#define KEY_UP                  0x41
#define KEY_DOWN                0x42
#define KEY_RIGHT               0x43
#define KEY_LEFT                0x44
#define KEY_BACKSPACE           127            /* [backspace] key */


//#define DEBUG_PRINT printf
#define DEBUG_PRINT shell_printf

//--------------------------------------------------
int shell_should_exit = 0;
//所有支持的命令
cmd_t cmd_tbl[] = {
    {
        .cmd = "help",
        .func = help_func
    },
    {
        .cmd = "echo",
        .func = echo_func
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
            return "";
    }
    return "";
}

int sh_strlen(char *str) {
    int i = 0;
    while(*str++ != '\0')
        i++;
    return i;
}

int auto_complete(char *input, int *idx) {
    DEBUG_PRINT("warning! auto_complete not implement yet,(%s),idx is %d\n", input, *idx);
    return 0;
}

int history_process(int history, char *input, int *idx, char *expand_buf, int *idx_ex) {
    DEBUG_PRINT("\nhistory %d, input %s, idx %d,     %d,expand_buf %s\n", history, input, *idx, *idx_ex, &expand_buf[*idx_ex]);
    int cmd_idx = (cmd_records.cmd_idx + history) % HISTORY_MAXITEMS;
    DEBUG_PRINT("target:%s\n", cmd_records.cmd_buf[cmd_idx]);
    //int cmd_idx = 0;
    //int i = 0;
    //if (history < 0 && (-history) < cmd_records.level) {
    //    //先移动到行的开始
    //    for (i = 0; i < *idx; i++) {
    //        shell_putchar('\b');
    //    }
    //    //刷掉目前行中显示的东西
    //    for (i = 0; i < *idx; i++) {
    //        shell_putchar(' ');
    //    }
    //    for (i = *idx_ex+1; expand_buf[i] > 0; i++) {
    //        shell_putchar(' ');
    //    }
    //    memset(input, 0, COMMAND_BUF_SIZE);
    //    memset(expand_buf, 0, COMMAND_BUF_SIZE);
    //    *idx = 0;
    //    *idx_ex = 0;
    //    cmd_idx = (cmd_records.cmd_idx + history) % HISTORY_MAXITEMS;
    //    memcpy(input, cmd_records.cmd_buf[cmd_idx], COMMAND_BUF_SIZE);
    //    *idx = sh_strlen(input);
    //    //再回到行的开始
    //    for (i = 0; i < *idx; i++) {
    //        shell_putchar('\b');
    //    }
    //    for (i = *idx_ex+1; expand_buf[i] > 0; i++) {
    //        shell_putchar('\b');
    //    }
    //    //把历史记录里的命令刷出来
    //    for (i = 0; i < *idx; i++) {
    //        shell_putchar(input[i]);
    //    }
    //}
    return 0;
}

int read_input(char *input)
{
    int i = 0;
    int ii = 0;
    char c;
    //当按下左右按键的时候,起始是光标移动到指定位置并插入
    char expand_buf[COMMAND_BUF_SIZE] = {0};
    int ie = COMMAND_BUF_SIZE - 2;  //最后一个留给'\0'
    int history = 0;    //0表示当前,-1表示上一个,-2表示前两个...

    while (1) {
        c = shell_getch();
        //DEBUG_PRINT("info! %d pressed!\n",c);

        if (c == '\n' || c == '\r') {
            shell_putchar('\r');
            shell_putchar('\n');
            break;
        }
        if (c == KEY_BACKSPACE) {
            if (i > 0) {
                shell_putchar('\b');
                if (expand_buf[ie+1] > 0) {
                    for (ii = ie+1; expand_buf[ii] > 0; ii++)
                        shell_putchar(expand_buf[ii]);
                    shell_putchar(' ');
                    for (ii = ie+1; expand_buf[ii] > 0; ii++)
                        shell_putchar('\b');
                    shell_putchar('\b');
                } else {
                    shell_putchar(' ');
                    shell_putchar('\b');
                }
                i--;
            }
            continue;
        }
        if (c == 0x1b) {
            c = shell_getch();
            if (c == 0x5b) {
                c = shell_getch();
                switch(c){
                    case KEY_UP:
                        //DEBUG_PRINT("up key pressed\n");
                        //当按下up/down按键,本次输入就算作废了
                        history--;
                        if (-history > cmd_records.level) {
                            history = -cmd_records.level;
                        }
                        history_process(history, input, &i, expand_buf, &ie);
                        break;
                    case KEY_DOWN:
                        //DEBUG_PRINT("down key pressed\n");
                        //当按下up/down按键,本次输入就算作废了
                        history++;
                        if (history >= -1) {
                            history = -1;
                        }
                        history_process(history, input, &i, expand_buf, &ie);
                        break;
                    case KEY_LEFT:
                        if (i > 0) {
                            shell_putchar('\b');
                            i--;
                            expand_buf[ie] = input[i];
                            ie--;
                        }
                        break;
                    case KEY_RIGHT:
                        if (expand_buf[ie+1] > 0) {
                            shell_putchar(expand_buf[ie+1]);   //putchar就是光标右移了,只不过这个char不能随便打印
                            input[i] = expand_buf[ie+1];
                            expand_buf[ie+1] = '\0';
                            i++;
                            ie++;
                        }
                        break;
                }
            }
            continue;
        }
        if (c == '\t') {
            //DEBUG_PRINT("info! tab pressed!\n");
            input[i++] = '\0';  //让auto_complete()继续填充
            auto_complete(input, &i);
            continue;
        }
        /* allocate more memory for input */
        if (i >= COMMAND_BUF_SIZE) {
            DEBUG_PRINT("error! input exceed COMMAND_BUF_SIZE(%d)\n", COMMAND_BUF_SIZE);
            return -1;
        }
        shell_putchar(c);
        //可能我们的光标并不在最末尾(因为前面可能已经按了左右移动光标的按键了)
        //这里需要将后面的也打印出来,且完成后光标还回到当前这里
        for (ii = ie+1; expand_buf[ii] > 0; ii++)
            shell_putchar(expand_buf[ii]);
        for (ii = ie+1; expand_buf[ii] > 0; ii++)
            shell_putchar('\b');
        input[i++] = c;
    }
    for (ii = ie+1; expand_buf[ii] > 0; ii++) {
        input[i++] = expand_buf[ii];
    }
    input[i] = '\0';
    return 0;
}

int is_blank(char *input)
{
    int n = (int) sh_strlen(input);
    int i;

    for (i = 0; i < n; i++) {
        if (input[i] != ' ')
            return 0;
    }
    return 1;
}

int parse_command(char *input, cmd_parsed_t *output)
{
    int n = (int) sh_strlen(input);
    int i, oi = 0;
    output->argc = 0;
    char flag = 0;  //echo "hello fang" 这种case,parse会得到argc=3
    for (i = 0; i < n; i++) {
        if (input[i] != ' ' || flag == 1) { //flag=1: 我们正处于一个""中
            if (oi >= COMMAND_BUF_SIZE) {
                DEBUG_PRINT("error: args too long!\n");
                return -1;
            }
            if (output->argc >= ARG_MAX_COUNT) {
                DEBUG_PRINT("error: too many args !\n");
                return -1;
            }
            if (input[i] == '\"') {
                if (flag == 0)
                    flag = 1;
                else
                    flag = 0;
            }
            output->args[output->argc][oi++] = input[i];
            if (i == n-1) { //到了末尾不管是不是空格都要完成
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
    int n = (int) sh_strlen(a);
    int i;
    if (n != (int) sh_strlen(b)) {
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
    int input_key_code = 0;
    cmd_tbl_items = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);
    char cur_cmd_buf[COMMAND_BUF_SIZE] = {0};
    cli_status_t rc = CLI_OK;

    while (shell_should_exit == 0) {
        shell_printf(">>");
        input_key_code = read_input(cur_cmd_buf);
        if (input_key_code < 0) {
            DEBUG_PRINT("error! read_input error\n");
            continue;
        } else if (input_key_code > 0) {
            DEBUG_PRINT("read input_key_code %d\n", input_key_code);
            return -1;
        }

        ////DEBUG
        //DEBUG_PRINT("cur_cmd_buf %s\n", cur_cmd_buf);

        if (sh_strlen(cur_cmd_buf) > 0 && !is_blank(cur_cmd_buf)) {
            cmd_parsed_t cmd_parsed = {0};

            if (parse_command(cur_cmd_buf, &cmd_parsed) != 0) {
                DEBUG_PRINT("error! parse_command error\n");
                continue;
            }

            ////DEBUG
            //for (int t = 0; t < cmd_parsed.argc; t++) {
            //    DEBUG_PRINT("argv%d:%s\n",t,cmd_parsed.argv[t]);
            //}
            ////DEBUG

            if (!match_cmd("history", cmd_parsed.argv[0])) {
                //add to history
                memcpy(cmd_records.cmd_buf[cmd_records.cmd_idx], cur_cmd_buf, sizeof(cur_cmd_buf));
                cmd_records.cmd_idx = (cmd_records.cmd_idx + 1) % HISTORY_MAXITEMS;
                if (cmd_records.level < HISTORY_MAXITEMS) {
                    cmd_records.level++;
                }
            }

            if ((rc = execute_command(&cmd_parsed)) != CLI_OK) {
                shell_printf("%s\n", status_to_str(rc));
            }
        }
    }
    return 0;
}

