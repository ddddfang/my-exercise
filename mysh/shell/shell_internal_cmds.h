
#ifndef __SHELL_INTERNAL_CMDS_H
#define __SHELL_INTERNAL_CMDS_H




void help_usage();
cli_status_t help_func(int argc, char *argv[]);

void echo_usage();
cli_status_t echo_func(int argc, char *argv[]);

void history_usage();
cli_status_t history_func(int argc, char *argv[]);

void exit_usage();
cli_status_t exit_func(int argc, char *argv[]);

#endif

