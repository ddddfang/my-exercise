#include "shell.h"
#include "shell_port.h"


//ascii
// [up] key: 0x1b 0x5b 0x41
// [down] key: 0x1b 0x5b 0x42
// [right] key: 0x1b 0x5b 0x43
// [left] key: 0x1b 0x5b 0x44
// [delete] 27 91 51 126 --> 1b 5b 33 7e
// [ins]    27 91 50 126 --> 1b 5b 32 7e
// [page u] 27 91 53 126
// [page d] 27 91 54 126
#define KEY_UP                  0x41
#define KEY_DOWN                0x42
#define KEY_RIGHT               0x43
#define KEY_LEFT                0x44
#define KEY_DEL                 0x33
#define KEY_BACKSPACE           127            /* [backspace] key */


//#define DEBUG_PRINT printf
#define DEBUG_PRINT shell_printf

//--------------------------------------------------
int shell_should_exit = 0;
//所有支持的命令
cmd_t cmd_tbl[] = {
    {
        .cmd = "help",
        .usage = help_usage,
        .func = help_func
    },
    {
        .cmd = "echo",
        .usage = echo_usage,
        .func = echo_func
    },
    {
        .cmd = "history",
        .usage = history_usage,
        .func = history_func
    },
    {
        .cmd = "exit",
        .usage = exit_usage,
        .func = exit_func
    }
};
int cmd_tbl_items = 0;
//命令执行记录
cmd_records_t cmd_records;
//支持 auto_complete 的关键字
shell_keywords_t shell_keywords;
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

//返回match了几个,他们的index存放在数组idxs中
static int find_match(char *in, int *idxs)
{
    int len = sh_strlen(in);
    int i = 0;
    int t = 0;
    int res = 0;
    for (i = 0; i < shell_keywords.keywords_cnt; i++) {
        for (t = 0; t < len; t++) {
            if (in[t] != shell_keywords.keywords_buf[i][t])
                break;
        }
        if (t == len) {
            idxs[res++] = i;
        }
    }
    return res;
}

static int auto_complete(char *buf, int *pi)
{
    int match_cnt = 0;
    int i = 0;
    //shell_printf("\033[1A"); //回到上一行

    //那些匹配的关键字 的 index
    int idxs[MAX_KEY_WORD_CNT];
    sh_memset(idxs, 0, MAX_KEY_WORD_CNT);

    char tmp[COMMAND_BUF_SIZE];
    sh_memset(tmp, 0, COMMAND_BUF_SIZE);

    for (i = 0; i < (*pi); i++) {
        tmp[i] = buf[i];
    }

    match_cnt = find_match(tmp, idxs);
    if (match_cnt > 1) {
        //若匹配有多个匹配项,打印他们.
        shell_printf("\r\n");
        for (i = 0; i < match_cnt; i++) {
            //idxs[i] is the index
            shell_printf("%s  ", shell_keywords.keywords_buf[idxs[i]]);
        }
        //打印传进来的字符串,不要修改,等下一次tab 直到 match_cnt 为1
        shell_printf("\r\n>>%s", tmp);
    } else if (match_cnt == 1) {
        //直接修改 buf 和 *pi
        //shell_printf("%s ", shell_keywords.keywords_buf[idxs[0]]);
        for (i = (*pi); i < sh_strlen(shell_keywords.keywords_buf[idxs[0]]); i++) {
            //
            buf[i] = shell_keywords.keywords_buf[idxs[0]][i];
            (*pi)++;
            shell_putchar(shell_keywords.keywords_buf[idxs[0]][i]);
        }
        buf[(*pi)] = '\0';
    } else {
        //没有找到匹配项那就什么都不要做
    }
    return 0;
}

static int key_backspace_process(char *buf, int *pi, char *exbuf, int *piex)
{
    int ii = 0;
    if (*pi > 0) {
        shell_putchar('\b');
        if (exbuf[*piex+1] > 0) {
            //exbuf 中的内容跟进
            for (ii = *piex + 1; exbuf[ii] > 0; ii++)
                shell_putchar(exbuf[ii]);
            shell_putchar(' ');
            //完成后回到光标原先的位置
            for (ii = *piex + 1; exbuf[ii] > 0; ii++)
                shell_putchar('\b');
            shell_putchar('\b');
        } else {
            shell_putchar(' ');
            shell_putchar('\b');
        }
        buf[(*pi)] = '\0';
        (*pi) = (*pi) - 1;
    }
    return 0;
}

static int key_left_process(char *buf, int *pi, char *exbuf, int *piex)
{
    if (*pi > 0) {
        shell_putchar('\b');

        (*pi) = (*pi) - 1;
        exbuf[(*piex)] = buf[(*pi)];
        (*piex) = (*piex) - 1;

        buf[(*pi)] = '\0';
    }
    return 0;
}

static int key_right_process(char *buf, int *pi, char *exbuf, int *piex)
{
    if (exbuf[(*piex) + 1] > 0) {
        shell_putchar(exbuf[(*piex) + 1]);   //putchar就是光标右移了,只不过这个char不能随便打印
        buf[(*pi)] = exbuf[(*piex) + 1];
        exbuf[(*piex) + 1] = '\0';

        (*pi) = (*pi) + 1;
        (*piex) = (*piex) + 1;
    }
    return 0;
}

static int key_del_process(char *buf, int *pi, char *exbuf, int *piex)
{
    if (exbuf[(*piex) + 1] > 0) {
        key_right_process(buf, pi, exbuf, piex);
        key_backspace_process(buf, pi, exbuf, piex);
    }
    return 0;
}

static int history_process(int offset, char *buf, int *pi, char *exbuf, int *piex) {
    int cmd_idx = (cmd_records.cmd_idx + offset) % HISTORY_MAXITEMS;
    int i = 0;

    //先移动到行的开始
    for (i = 0; i < (*pi); i++) {
        shell_putchar('\b');
    }
    //刷掉目前行中显示的东西
    for (i = 0; i < (*pi); i++) {
        shell_putchar(' ');
    }
    for (i = (*piex) + 1; exbuf[i] > 0; i++) {
        shell_putchar(' ');
    }
    //再回到行的开始
    for (i = 0; i < (*pi); i++) {
        shell_putchar('\b');
    }
    for (i = (*piex) + 1; exbuf[i] > 0; i++) {
        shell_putchar('\b');
    }

    sh_memset(exbuf, 0, COMMAND_BUF_SIZE);
    *piex = COMMAND_BUF_SIZE - 2;  //最后一个留给'\0'
    sh_memcpy(buf, cmd_records.cmd_buf[cmd_idx], COMMAND_BUF_SIZE);
    *pi = sh_strlen(buf);

    //把历史记录里的命令刷出来
    for (i = 0; i < (*pi); i++) {
        shell_putchar(buf[i]);
    }
    return 0;
}

int read_input(char *input)
{
    int i = 0;
    int ii = 0;
    char c;

    //当按下左右按键的时候,起始是光标移动到指定位置并插入
    //char expand_buf[COMMAND_BUF_SIZE] = {0};    //这会隐式调用memset
    char expand_buf[COMMAND_BUF_SIZE];
    sh_memset(expand_buf, 0, COMMAND_BUF_SIZE);
    int ie = COMMAND_BUF_SIZE - 2;  //最后一个留给'\0'

    int offset = 0;    //0表示当前,-1表示上一个,-2表示前两个...

    while (1) {
        c = shell_getch();
        //DEBUG_PRINT("info! %d pressed!\n",c);

        if (c == '\n' || c == '\r') {
            shell_putchar('\r');
            shell_putchar('\n');
            break;
        }
        if (c == KEY_BACKSPACE) {
            key_backspace_process(input, &i, expand_buf, &ie);
            continue;
        }
        if (c == 0x1b) {
            c = shell_getch();
            if (c == 0x5b) {
                c = shell_getch();
                switch(c){
                    case KEY_UP:    //当按下up/down按键,本次输入就算作废了
                        offset--;
                        if (-offset > cmd_records.level) {
                            offset = -cmd_records.level;
                        }
                        history_process(offset, input, &i, expand_buf, &ie);
                        break;
                    case KEY_DOWN:
                        offset++;
                        if (offset >= -1) {
                            offset = -1;
                        }
                        history_process(offset, input, &i, expand_buf, &ie);
                        break;
                    case KEY_LEFT:
                        key_left_process(input, &i, expand_buf, &ie);
                        break;
                    case KEY_RIGHT:
                        key_right_process(input, &i, expand_buf, &ie);
                        break;
                    case KEY_DEL:
                        shell_getch();//discard
                        key_del_process(input, &i, expand_buf, &ie);
                        break;
                }
            }
            continue;
        }
        if (c == '\t') {
            //input[i++] = '\0';  //让auto_complete()继续填充
            auto_complete(input, &i);
            continue;
        }
        if (i >= COMMAND_BUF_SIZE) {
            DEBUG_PRINT("error! input exceed COMMAND_BUF_SIZE(%d)\n", COMMAND_BUF_SIZE);
            return -1;
        }
        shell_putchar(c);
        input[i++] = c;
        //可能我们的光标并不在最末尾(因为前面可能已经按了左右移动光标的按键了)
        //这里需要将后面的也打印出来,且完成后光标还回到当前这里
        for (ii = ie + 1; expand_buf[ii] > 0; ii++)
            shell_putchar(expand_buf[ii]);
        for (ii = ie + 1; expand_buf[ii] > 0; ii++)
            shell_putchar('\b');
    }
    //将expand_buf中的内容append到input后再返回
    for (ii = ie + 1; expand_buf[ii] > 0; ii++) {
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
    cli_status_t rc = CLI_OK;
    //int len = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);
    int len = cmd_tbl_items;

    for (i = 0; i < len; i++) {
        if (match_cmd(cmd_tbl[i].cmd, cmd->argv[0])) {
            if (cmd_tbl[i].func)
                rc = (*cmd_tbl[i].func)(cmd->argc, cmd->argv);
                if (rc == CLI_INVALID_ARGS) {   //print usage
                    if (cmd_tbl[i].usage)
                        (*cmd_tbl[i].usage)();
                }
                return rc;
        }
    }
    return CLI_CMD_NOT_FOUND;
}

int shell_init()
{
    int i = 0;
    cmd_tbl_items = sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);
    shell_printf_init();
    shell_printf("********************************************************************\r\n");
    shell_printf("** this is a simple cli, type 'help' to see what cmds is supported.\r\n");
    shell_printf("********************************************************************\r\n");

    for (i = 0; i < cmd_tbl_items; i++) {
        sh_memcpy(shell_keywords.keywords_buf[i], cmd_tbl[i].cmd, sh_strlen(cmd_tbl[i].cmd));
        shell_keywords.keywords_cnt++;
    }

    DEBUG_PRINT("all supported keywords are { ");
    for (i = 0; i < shell_keywords.keywords_cnt; i++) {
        DEBUG_PRINT("%s ", shell_keywords.keywords_buf[i]);
    }
    DEBUG_PRINT("}\r\n");

    shell_printf("\r\n");
    return 0;
}

int shell_execute()
{
    int input_key_code = 0;
    //char cur_cmd_buf[COMMAND_BUF_SIZE] = {0};
    char cur_cmd_buf[COMMAND_BUF_SIZE];
    cli_status_t rc = CLI_OK;

    while (shell_should_exit == 0) {
        shell_printf(">>");

        sh_memset(cur_cmd_buf, 0, COMMAND_BUF_SIZE);
        input_key_code = read_input(cur_cmd_buf);
        if (input_key_code < 0) {
            DEBUG_PRINT("error! read_input error\r\n");
            continue;
        } else if (input_key_code > 0) {
            DEBUG_PRINT("read input_key_code %d\r\n", input_key_code);
            return -1;
        }

        ////DEBUG
        //DEBUG_PRINT("cur_cmd_buf %s\r\n", cur_cmd_buf);

        if (sh_strlen(cur_cmd_buf) > 0 && !is_blank(cur_cmd_buf)) {
            //cmd_parsed_t cmd_parsed = {0};
            cmd_parsed_t cmd_parsed;
            sh_memset(&cmd_parsed, 0, sizeof(cmd_parsed));

            if (parse_command(cur_cmd_buf, &cmd_parsed) != 0) {
                DEBUG_PRINT("error! parse_command error\r\n");
                continue;
            }

            ////DEBUG
            //for (int t = 0; t < cmd_parsed.argc; t++) {
            //    DEBUG_PRINT("argv%d:%s\r\n",t,cmd_parsed.argv[t]);
            //}
            ////DEBUG

            if (!match_cmd("history", cmd_parsed.argv[0])) {
                //add to history
                sh_memcpy(cmd_records.cmd_buf[cmd_records.cmd_idx], cur_cmd_buf, sizeof(cur_cmd_buf));
                cmd_records.cmd_idx = (cmd_records.cmd_idx + 1) % HISTORY_MAXITEMS;
                if (cmd_records.level < HISTORY_MAXITEMS) {
                    cmd_records.level++;
                }
            }

            if ((rc = execute_command(&cmd_parsed)) != CLI_OK) {
                shell_printf("---------------------\r\n");
                shell_printf("error: %s\r\n", status_to_str(rc));
                shell_printf("---------------------\r\n");
            }
        }
    }
    return 0;
}

int main() {

    shell_init();

    return shell_execute();
}

