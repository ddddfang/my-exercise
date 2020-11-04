
#ifndef __SHELL_PORT_H
#define __SHELL_PORT_H


int sh_strlen(char *str);
int sh_memset(void *p, int val, int size);
int sh_memcpy(void *dst, void *src, int size);

int shell_getch(void);
int shell_putchar(char ch);
int shell_printf(const char *restrict format, ...);

cli_status_t help_func(int argc, char *argv[]);
cli_status_t echo_func(int argc, char *argv[]);
cli_status_t history_func(int argc, char *argv[]);
cli_status_t exit_func(int argc, char *argv[]);

#endif

