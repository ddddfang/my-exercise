#include "ufat.h"
#include "shell/shell.h"
#include "common/printf.h"


struct ufat_device g_ufat_dev;
#include <stdio.h>
int read_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, void *buffer) {
    FILE *fp = fopen("./test.fat32", "r+b");

    fseek(fp, start << g_ufat_dev.log2_block_size, SEEK_SET);
    fread(buffer, 1 << g_ufat_dev.log2_block_size, count, fp);
    fclose(fp);
    return 0;
}

int write_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, const void *buffer) {
    FILE *fp = fopen("./test.fat32", "r+");

    fseek(fp, start << g_ufat_dev.log2_block_size, SEEK_SET);
    fwrite(buffer, 1 << g_ufat_dev.log2_block_size, count, fp);
    fclose(fp);
    return 0;
}

struct ufat_device g_ufat_dev = {    //fang: sector size, read and write ?
    .log2_block_size = 9,
    .read = read_sector,
    .write = write_sector,
};

//void echo_usage() {
//    shell_printf("usage: echo <string>\r\n");
//}
//
//cli_status_t echo_func(int argc, char *argv[]) {
//    if (argc != 2) {
//        return CLI_INVALID_ARGS;
//    }
//    shell_printf("%s\r\n", argv[1]);
//    return CLI_OK;
//}

void format_device_usage() {
    shell_printf("usage: format device(mkfs32).\r\n");
}

cli_status_t format_device_func(int argc, char *argv[]) {
    int i = 0;
    //shell_printf("help_func execute, args are:\r\n");
    //for (i = 0; i < argc; i++) {
    //    shell_printf("arg%d: %s\r\n", i, argv[i]);
    //}
    if (argc != 1) {
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
    //err = ufat_mkfs(&dev.base, opt.num_blocks);
    return CLI_OK;
}





