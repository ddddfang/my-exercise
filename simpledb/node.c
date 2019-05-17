#include "node.h"

//fang:
//the Node is exactly lies in a page. Node is page's logic name.


/*************************************** common node related utils **************************************/
//get NODE_TYPE
NodeType get_node_type(void* node) {
	uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
	return (NodeType)value;
}

//set NODE_TYPE
void set_node_type(void* node, NodeType type) {
	uint8_t value = type;
	*((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

bool is_node_root(void* node) {
	uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));
	return (bool)value;
}

void set_node_root(void* node, bool is_root) {
	uint8_t value = (uint8_t)is_root;
	*((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}

uint32_t* node_parent(void* node) {
	return (uint32_t *)(node + PARENT_POINTER_OFFSET);
}

//given a page(node)
//	return the max key in this node(always the rightst)
uint32_t get_node_max_key(void* node) {
	switch (get_node_type(node)) {
		case NODE_INTERNAL:
			return *internal_node_key(node, *internal_node_num_keys(node) - 1);
		case NODE_LEAF:
			return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
	}
	return 0;
}

/*************************************** leaf node related utils **************************************/
//initialize leaf node
void initialize_leaf_node(void* node) {
	set_node_type(node, NODE_LEAF);
	set_node_root(node, false);	//todo : set parent page num
	*leaf_node_num_cells(node) = 0;
	*leaf_node_next_leaf(node) = 0;	//0 represents no sibling
}

//given a page(node),
//	return position of LEAF_NODE_NUM_CELLS in this node
uint32_t* leaf_node_num_cells(void* node) {
	return (uint32_t *)((char *)node + LEAF_NODE_NUM_CELLS_OFFSET);
}

//given a page(node) and cell's number,
//	return position of this cell(kv) in this node
void* leaf_node_cell(void* node, uint32_t cell_num) {
	return (void *)((char *)node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE);
}

//given a page(node) and cell's number,
//	return position of this cell's key in this node
uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
	return (uint32_t *)leaf_node_cell(node, cell_num);
}

//given a page(node) and cell's number,
//	return position of this cell's value in this node
void* leaf_node_value(void* node, uint32_t cell_num) {
	return (void *)(leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE);
}

uint32_t* leaf_node_next_leaf(void* node) {
	return (uint32_t *)(node + LEAF_NODE_NEXT_LEAF_OFFSET);
}

/*************************************** internal node related utils **************************************/
//initialize internal node
void initialize_internal_node(void* node) {
	set_node_type(node, NODE_INTERNAL);
	set_node_root(node, false);//todo : set parent page num
	*internal_node_num_keys(node) = 0;
}

//given a page(node),
//	return position of INTERNAL_NODE_NUM_KEYS in this node
uint32_t* internal_node_num_keys(void* node) {
	return (uint32_t *)(node + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

//given a page(node),
//	return position of the first child field in this node
uint32_t* internal_node_right_child(void* node) {
	return (uint32_t *)(node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

//given a page(node) and cell's number,
//	return position of this cell(k+child) in this node
uint32_t* internal_node_cell(void* node, uint32_t cell_num) {
	return (uint32_t *)(node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE);
}

//given a page(node) and child's number,(child is always 1 more greater than k)
//	return the p->"page num" of this child in this node
//NOTICE: the index of child in header field is "num_keys"(it's the last child), others is [0,num_keys)
uint32_t* internal_node_child(void* node, uint32_t child_num) {
	uint32_t num_keys = *internal_node_num_keys(node);
	if (child_num > num_keys) {	//child_num is in [0,num_keys], emmm...
		printf("Error: Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
		exit(EXIT_FAILURE);
	} else if (child_num == num_keys) {
		return internal_node_right_child(node);
	} else {
		return internal_node_cell(node, child_num);
	}
}

//given a page(node) and key's number,
//	return the p->"this key" in this node
uint32_t* internal_node_key(void* node, uint32_t key_num) {
	return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}


