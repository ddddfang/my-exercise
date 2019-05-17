#ifndef __NODE_BTREE_H
#define __NODE_BTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "config.h"
#include "table.h"
#include "node.h"	//
#include "row.h"	//leaf_node_insert


void create_new_root(Table* table, uint32_t right_child_page_num);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key);
void internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num);

/*  */
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);
Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key);

#endif

