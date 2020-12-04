#include "shell/shell.h"
#include "common/printf.h"
#include "fs/vfat.h"
#include "common/block.h"

int main() {
    int rc;
    block_device_init();

    shell_printf_init();

    //fs_func();
    shell_init();

    rc = shell_execute();

    block_device_deinit();
}

