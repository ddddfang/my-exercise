#ifndef __META_H
#define __META_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "inputs.h"
#include "table.h"
#include "row.h"
#include "node_btree.h"

typedef enum MetaCommandResult_t {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

int is_meta_command(InputBuffer * input_buffer);
MetaCommandResult do_meta_command(InputBuffer * input_buffer, Table *table);
void print_constants(Table *table);
void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);

#endif

