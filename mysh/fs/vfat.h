#ifndef __VFAT_H
#define __VFAT_H


#include "shell/shell.h"

void ufat_usage();
cli_status_t ufat_func(int argc, char *argv[]);

void touch_usage();
cli_status_t touch_func(int argc, char *argv[]);

void mkdir_usage();
cli_status_t mkdir_func(int argc, char *argv[]);

void sync_usage();
cli_status_t sync_func(int argc, char *argv[]);

void ls_usage();
cli_status_t ls_func(int argc, char *argv[]);

void cd_usage();
cli_status_t cd_func(int argc, char *argv[]);


//int fs_func();



#endif

