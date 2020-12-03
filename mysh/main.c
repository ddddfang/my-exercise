#include "shell/shell.h"
#include "common/printf.h"

int main() {

    shell_printf_init();
    shell_init();

    return shell_execute();
}

