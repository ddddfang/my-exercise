#include "meta.h"

//fang:
//this layer execute virtual machine meta related commands.


int is_meta_command(InputBuffer * input_buffer) {
	if(input_buffer->buffer[0] == '.')
		return 1;
	return 0;
}

//we do meta command in this func
MetaCommandResult do_meta_command(InputBuffer * input_buffer, Table *table) {
	if(strcmp(input_buffer->buffer,".exit") == 0) {
		db_close(table);
		printf("Bye.\n");
		exit(EXIT_SUCCESS);
	} else if (strcmp(input_buffer->buffer, ".constants") == 0) {
		printf("Constants:\n");
		print_constants(table);
		return META_COMMAND_SUCCESS;
	} else if (strcmp(input_buffer->buffer, ".btree") == 0) {
		printf("Tree:\n");
		print_tree(table->pager, 0, 0);
		return META_COMMAND_SUCCESS;
	} else {
		return META_COMMAND_UNRECOGNIZED;
	}
}

void print_constants(Table *table) {
	printf("ROW_SIZE: %lu\n", ROW_SIZE);
	printf("COMMON_NODE_HEADER_SIZE: %lu\n", COMMON_NODE_HEADER_SIZE);
	printf("LEAF_NODE_HEADER_SIZE: %lu\n", LEAF_NODE_HEADER_SIZE);
	printf("LEAF_NODE_CELL_SIZE: %lu\n", LEAF_NODE_CELL_SIZE);
	printf("LEAF_NODE_SPACE_FOR_CELLS: %lu\n", LEAF_NODE_SPACE_FOR_CELLS);
	printf("LEAF_NODE_MAX_CELLS: %lu\n", LEAF_NODE_MAX_CELLS);
	printf("root_page_num: %u\n", table->root_page_num);
}

void indent(uint32_t level) {
	for (uint32_t i = 0; i < level; i++) {
		printf("  ");
	}
}

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
	void* node = pager_get_page(pager, page_num);
	uint32_t num_keys, child;

	switch (get_node_type(node)) {
		case (NODE_LEAF):
			num_keys = *leaf_node_num_cells(node);
			indent(indentation_level);
			printf("- leaf (size %d)\n", num_keys);
			for (uint32_t i = 0; i < num_keys; i++) {
				indent(indentation_level + 1);
				printf("- %d\n", *leaf_node_key(node, i));
			}
			break;
		case (NODE_INTERNAL):
			num_keys = *internal_node_num_keys(node);
			indent(indentation_level);
			printf("- internal (size %d)\n", num_keys);
			for (uint32_t i = 0; i < num_keys; i++) {
				child = *internal_node_child(node, i);
				print_tree(pager, child, indentation_level + 1);

				indent(indentation_level);
				printf("- key %d\n", *internal_node_key(node, i));
			}
			child = *internal_node_right_child(node);
			print_tree(pager, child, indentation_level + 1);
			break;
	}
}


