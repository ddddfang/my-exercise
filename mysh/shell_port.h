
#ifndef __SHELL_PORT_H
#define __SHELL_PORT_H


int shell_getch(void);
int shell_putchar(char ch);
int shell_printf(const char *restrict format, ...);

cli_status_t help_func(int argc, char *argv[]);
cli_status_t history_func(int argc, char *argv[]);
cli_status_t exit_func(int argc, char *argv[]);

#endif

