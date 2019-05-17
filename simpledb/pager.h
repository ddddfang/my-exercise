#ifndef __PAGER_H
#define __PAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "config.h"

typedef struct Pager_t {
	int file_descriptor;
	uint32_t file_length;
	uint32_t num_pages;
	void* pages[TABLE_MAX_PAGES];
} Pager;

Pager* pager_open(const char* filename);
void pager_flush(Pager* pager, uint32_t page_num);
void* pager_get_page(Pager* pager, uint32_t page_num);
uint32_t pager_get_unused_page_num(Pager* pager);

#endif

