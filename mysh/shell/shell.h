#ifndef __SHELL_H
#define __SHELL_H



typedef enum
{
    CLI_OK,
    CLI_CMD_NOT_FOUND,
    CLI_INVALID_ARGS,
    CLI_EXE_ERROR
} cli_status_t;

typedef cli_status_t (*cmd_func_ptr_t)(int argc, char *argv[]);
typedef void (*cmd_usage_ptr_t)();

//一条内建命令
typedef struct
{
    char *cmd;
    cmd_usage_ptr_t usage;
    cmd_func_ptr_t func;
} cmd_t;


#define COMMAND_BUF_SIZE 2048
#define ARG_MAX_COUNT    10
#define HISTORY_MAXITEMS 50

//命令记录
typedef struct {
    int cmd_idx;
    int level;
    char cmd_buf[HISTORY_MAXITEMS][COMMAND_BUF_SIZE];
} cmd_records_t;

//一条命令记录(parse过的)
//关于为什么要argv/args, https://bbs.csdn.net/topics/391014879
typedef struct {
	int argc;
	char *argv[ARG_MAX_COUNT];
	char args[ARG_MAX_COUNT][COMMAND_BUF_SIZE];
} cmd_parsed_t;


//关键字最大长度,用于 auto_complete
#define MAX_KEY_WORD_SIZE   32
//最大关键字个数
#define MAX_KEY_WORD_CNT    1024
typedef struct {
    int keywords_cnt;
    //int top_key_idx[MAX_KEY_WORD_CNT];
    //int next_key_idx[MAX_KEY_WORD_CNT];
    char keywords_buf[MAX_KEY_WORD_CNT][MAX_KEY_WORD_SIZE];
} shell_keywords_t;

char *status_to_str(cli_status_t st);
int read_input(char *input);
int parse_command(char *input, cmd_parsed_t *output);
int match_cmd(char *a, char *b);
cli_status_t execute_command(cmd_parsed_t *cmd);

int shell_set_prompt(char *p);
int shell_add_keyword(char *keyname);
int shell_remove_keyword(char *keyname);
int shell_init(void);
int shell_execute(void);

extern int shell_should_exit;
extern cmd_records_t cmd_records;

#endif
