#include "ufat.h"
#include "shell/shell.h"
#include "common/printf.h"
#include "common/block.h"
#include "common/lib_mem.h"


static struct ufat_device ufat_dev = {    //fang: sector size, read and write ?
    .log2_block_size = 9,
    .read = read_sector,
    .write = write_sector,
};
static struct ufat uf;  //代表当前打开的文件系统

static struct ufat_directory dir;   //代表当前所在的目录
#define FULL_PATH_LEN   2048
static char cur_dir_full[FULL_PATH_LEN];

//cd /home/fang/tmp-code/my-exercise/mysh
//dd if=/dev/zero of=test.fat32 count=2097152
//mkdir tmp
//sudo mount -o loop -tvfat test.fat32 ./tmp/

#define CHECK_FS_OPEN()   \
    if(!uf.dev) {\
        shell_printf("ufat has not open, you should open it first!\r\n");\
        return CLI_EXE_ERROR;\
    }

//---------------------------------------------------
void ufat_usage() {
    shell_printf("usage: ufat <cmd>\r\n");
    shell_printf("<cmd> can be 1.format 2.open 3.info\r\n");
}

cli_status_t ufat_func(int argc, char *argv[]) {
    int rc;
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    if (match_cmd("format", argv[1])) {
        rc = ufat_mkfs(&ufat_dev, 2097152);   //
        if (rc) {
            return CLI_EXE_ERROR;
        }
        return CLI_OK;
    } else if (match_cmd("open", argv[1])) {
        rc = ufat_open(&uf, &ufat_dev);
        if (rc) {
            return CLI_EXE_ERROR;
        }
        ufat_open_root(&uf, &dir);
        cur_dir_full[0] = '/';
        cur_dir_full[1] = '\0';
        shell_set_prompt(cur_dir_full);
        return CLI_OK;
    } else if (match_cmd("info", argv[1])) {
        CHECK_FS_OPEN();
        struct ufat_bpb *bpb = &uf.bpb;
        shell_printf("Type:                       FAT%d\r\n", bpb->type);
        shell_printf("Blocks per cluster:         %u\r\n", 1 << bpb->log2_blocks_per_cluster);
        shell_printf("FAT size (blocks):          %u\r\n", bpb->fat_size);
        shell_printf("FAT offset (block):         %u\r\n", bpb->fat_start);
        shell_printf("FAT count:                  %u\r\n", bpb->fat_count);
        shell_printf("Cluster starting block:     %u\r\n", bpb->cluster_start);
        shell_printf("Clusters:                   %u\r\n", bpb->num_clusters);
        shell_printf("Root directory block start: %u\r\n", bpb->root_start);
        shell_printf("Root directory block count: %u\r\n", bpb->root_size);
        shell_printf("Root directory cluster:     %u\r\n", bpb->root_cluster);
        return CLI_OK;
    }
    return CLI_INVALID_ARGS;
}

//---------------------------------------------------
void touch_usage() {
    shell_printf("usage: touch <filname>\r\n");
    shell_printf("create a empty file in current dir.\r\n");
}

cli_status_t touch_func(int argc, char *argv[]) {
    int rc;
    struct ufat_dirent inf;
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    CHECK_FS_OPEN();
    rc = ufat_dir_mkfile(&dir, &inf, argv[1]);
    if (rc) {
        return CLI_EXE_ERROR;
    }
    return CLI_OK;
}

//---------------------------------------------------
void mkdir_usage() {
    shell_printf("usage: mkdir <dirname>\r\n");
    shell_printf("create a empty dir in current dir.\r\n");
}

cli_status_t mkdir_func(int argc, char *argv[]) {
    int rc;
    struct ufat_dirent inf;
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    CHECK_FS_OPEN();
    rc = ufat_dir_create(&dir, &inf, argv[1]);
    if (rc) {
        return CLI_EXE_ERROR;
    }
    return CLI_OK;
}

//---------------------------------------------------
void sync_usage() {
    shell_printf("usage: sync\r\n");
    shell_printf("sync ufat system.\r\n");
}

cli_status_t sync_func(int argc, char *argv[]) {
    int rc;
    if (argc != 1) {
        return CLI_INVALID_ARGS;
    }
    CHECK_FS_OPEN();
    rc = ufat_sync(&uf);
    if (rc) {
        return CLI_EXE_ERROR;
    }
    return CLI_OK;
}

//---------------------------------------------------
void ls_usage() {
    shell_printf("usage: ls\r\n");
    shell_printf("list all items in cur directory.\r\n");
}

cli_status_t ls_func(int argc, char *argv[]) {
    int rc;

    if (argc != 1) {
        return CLI_INVALID_ARGS;
    }
    CHECK_FS_OPEN();
    ufat_dir_rewind(&dir);
    while (1) {
        struct ufat_dirent inf;
        char lfn[UFAT_LFN_MAX_UTF8];
        rc = ufat_dir_read(&dir, &inf, lfn, sizeof(lfn));
        if (rc < 0) {
            return CLI_EXE_ERROR;
        }
        if (rc) {   //reach the end
            break;
        }
        shell_printf("%04d-%02d-%02d ", 
            UFAT_DATE_Y(inf.modify_date), 
            UFAT_DATE_M(inf.modify_date),
            UFAT_DATE_D(inf.modify_date));
        shell_printf("%2d:%02d:%02d ",
            UFAT_TIME_H(inf.modify_time),
            UFAT_TIME_M(inf.modify_time),
            UFAT_TIME_S(inf.modify_time));

        shell_printf("%c%c%c%c%c ",
            (inf.attributes & UFAT_ATTR_ARCHIVE) ? 'A' : ' ',
            (inf.attributes & UFAT_ATTR_SYSTEM) ? 'S' : ' ',
            (inf.attributes & UFAT_ATTR_HIDDEN) ? 'H' : ' ',
            (inf.attributes & UFAT_ATTR_READONLY) ? 'R' : ' ',
            (inf.attributes & UFAT_ATTR_DIRECTORY) ? 'D' : ' ');
        shell_printf("%9u ", inf.file_size);
        shell_printf("%s\r\n", lfn);
    }
    return CLI_OK;
}
//---------------------------------------------------
void cd_usage() {
    shell_printf("usage: cd <dirname>\r\n");
    shell_printf("enter a dir (which is in cur dir).\r\n");
}

cli_status_t cd_func(int argc, char *argv[]) {
    int rc;
    struct ufat_dirent inf;
    if (argc != 2) {
        return CLI_INVALID_ARGS;
    }
    CHECK_FS_OPEN();

    rc = ufat_dir_find(&dir, argv[1], &inf);
    if (rc) {
        shell_printf("cannot find this dir.\r\n");
        return CLI_EXE_ERROR;
    }
    rc = ufat_open_subdir(&uf, &dir, &inf);
    if (rc) {
        return CLI_EXE_ERROR;
    }

    //modify cur_dir_full[]
    if (match_cmd(".", argv[1])) {
    
    } else if (match_cmd("..", argv[1])) {
        int i = sh_strlen(cur_dir_full);
        while (cur_dir_full[i] != '/') 
            i--;
        if (i == 0) {   //means the only "/" root dir
            cur_dir_full[1] = '\0';
        } else {
            cur_dir_full[i] = '\0';
        }
        shell_set_prompt(cur_dir_full);
    } else {
        int orilen = sh_strlen(cur_dir_full);
        int subdirlen = sh_strlen(argv[1]);

        if (orilen + 1 + subdirlen > FULL_PATH_LEN) {   //1 is for divider '/'
            shell_printf("FULL_PATH_LEN limit!\r\n");
            return CLI_EXE_ERROR;
        }
        if (orilen != 1) {
            cur_dir_full[orilen] = '/'; // '/abc/def/hello'
            sh_memcpy(cur_dir_full + orilen + 1, argv[1], subdirlen);
            cur_dir_full[orilen + 1 + subdirlen] = '\0';
        } else {
            sh_memcpy(cur_dir_full + orilen, argv[1], subdirlen);
            cur_dir_full[orilen + subdirlen] = '\0';
        }

        shell_set_prompt(cur_dir_full);
    }
    return CLI_OK;
}

//int fs_func() {
//    int rc = 0;
//
//
//    rc = ufat_mkfs(&ufat_dev, 2097152);   //
//    if (rc < 0) {
//        shell_printf("mkfs error.\r\n");
//        return -1;
//    }
//    shell_printf("mkfs success.\r\n");
//
//    rc = ufat_open(&uf, &ufat_dev);
//    if (rc) {
//        shell_printf("ufat_open error.\r\n");
//        return -1;
//    }
//
//    ufat_open_root(&uf, &dir);
//
//    rc = ufat_dir_mkfile(&dir, &ent, "test_file_001.txt");
//    if (rc) {
//        shell_printf("ufat_dir_mkfile error.\r\n");
//        return -1;
//    } else {
//        shell_printf("ufat_dir_mkfile success\r\n");
//    }
//
//    rc = ufat_dir_create(&dir, &ent, "test_dir_001");
//    if (rc) {
//        shell_printf("ufat_dir_create error.\r\n");
//        return -1;
//    } else {
//
//        rc = ufat_open_subdir(&uf, &dir, &ent);
//        if (rc) {
//            shell_printf("ufat_dir_create error.\r\n");
//            return -1;
//        }
//        rc = ufat_dir_create(&dir, &ent, "test_sub_dir_001");
//        rc |= ufat_dir_mkfile(&dir, &ent, "test_sub_file_001.txt");
//        if (rc) {
//            shell_printf("ufat_dir_create / ufat_dir_mkfile error.\r\n");
//            return -1;
//        }
//    }
//
//
//
//    ufat_close(&uf);
//    return 0;
//}




