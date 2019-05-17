#ifndef __TABLE_H
#define __TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "pager.h"

typedef struct Table_t {
	Pager *pager;
	uint32_t root_page_num;	//
} Table;

typedef struct Cursor_t {
	Table* table;
	uint32_t page_num;
	uint32_t cell_num;
	bool end_of_table;  // Indicates a position one past the last element
} Cursor;


Table* db_open(const char* filename);
void db_close(Table* table);

Cursor* table_start(Table* table);
Cursor* table_find(Table* table, uint32_t key);
void* cursor_value(Cursor* cursor);
void cursor_advance(Cursor* cursor);


#endif

