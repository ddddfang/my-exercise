#ifndef __NODE_H
#define __NODE_H

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
#include "row.h"

typedef enum NodeType_t {
	NODE_INTERNAL,
	NODE_LEAF
} NodeType;


/*********************** Leaf Node layout *****************************/
/*
 * Common Node Header Layout
 */
//NODE_TYPE (uint8_t)
#define NODE_TYPE_OFFSET	(0)
#define NODE_TYPE_SIZE		(sizeof(uint8_t))
//IS_ROOT (uint8_t)
#define IS_ROOT_OFFSET 		(NODE_TYPE_OFFSET + NODE_TYPE_SIZE)
#define IS_ROOT_SIZE 		(sizeof(uint8_t))
//PARENT_POINTER (uint32_t)
#define PARENT_POINTER_OFFSET	(IS_ROOT_OFFSET + IS_ROOT_SIZE)
#define PARENT_POINTER_SIZE		(sizeof(uint32_t))
//(common header size)
#define COMMON_NODE_HEADER_SIZE (NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE)

/*
 * Leaf Node Header Layout
 */
//LEAF_NODE_NUM_CELLS (uint32_t). Each cell is a key followed by a value (a serialized row).
#define LEAF_NODE_NUM_CELLS_OFFSET	(COMMON_NODE_HEADER_SIZE)
#define LEAF_NODE_NUM_CELLS_SIZE	(sizeof(uint32_t))
//LEAF_NODE_NEXT_LEAF (uint32_t)
#define LEAF_NODE_NEXT_LEAF_OFFSET	(LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE)
#define LEAF_NODE_NEXT_LEAF_SIZE	(sizeof(uint32_t))
//(total header size) = (common header size) + (node header size)
#define LEAF_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE + LEAF_NODE_NEXT_LEAF_SIZE)

/*
 * Leaf Node Body Layout
 */
//LEAF_NODE_KEY
#define LEAF_NODE_KEY_OFFSET 	(0)
#define LEAF_NODE_KEY_SIZE 		(sizeof(uint32_t))
//LEAF_NODE_VALUE
#define LEAF_NODE_VALUE_OFFSET 	(LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE)
#define LEAF_NODE_VALUE_SIZE 	(ROW_SIZE)
//ceil size = leaf_node_key + leaf_node_value
#define LEAF_NODE_CELL_SIZE (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)

//conclusion
#define LEAF_NODE_SPACE_FOR_CELLS	(PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS			(LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)



//fang: when split, represents there is a new node is joined, so (LEAF_NODE_MAX_CELLS + 1) is updated cell num.
//so in general, right is 1 cell less than left.
#define LEAF_NODE_RIGHT_SPLIT_COUNT	((LEAF_NODE_MAX_CELLS + 1) / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT	((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)

/*********************** Internal Node layout *****************************/
/*
 * Internal Node Header Layout
 */
//INTERNAL_NODE_NUM_KEYS
#define INTERNAL_NODE_NUM_KEYS_OFFSET	(COMMON_NODE_HEADER_SIZE)
#define INTERNAL_NODE_NUM_KEYS_SIZE		(sizeof(uint32_t))
//INTERNAL_NODE_RIGHT_CHILD
#define INTERNAL_NODE_RIGHT_CHILD_OFFSET	(INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE)
#define INTERNAL_NODE_RIGHT_CHILD_SIZE		(sizeof(uint32_t))
//(total header size) = (common header size) + (num keys size) + (the first child pointer field size)
#define INTERNAL_NODE_HEADER_SIZE		(COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE)

/*
 * Internal Node Body Layout
 */
#define INTERNAL_NODE_CHILD_SIZE	(sizeof(uint32_t))
#define INTERNAL_NODE_KEY_SIZE		(sizeof(uint32_t))
#define INTERNAL_NODE_CELL_SIZE		(INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE)
/* Keep this small for testing */
#define INTERNAL_NODE_MAX_CELLS		3


/*  */
NodeType get_node_type(void* node);
void set_node_type(void* node, NodeType type);
bool is_node_root(void* node);
void set_node_root(void* node, bool is_root);
uint32_t* node_parent(void* node);
uint32_t get_node_max_key(void* node);

/*  */
void initialize_leaf_node(void* node);
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
uint32_t* leaf_node_next_leaf(void* node);

/*  */
void initialize_internal_node(void* node);
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node, uint32_t cell_num);
uint32_t* internal_node_child(void* node, uint32_t child_num);
uint32_t* internal_node_key(void* node, uint32_t key_num);

#endif

