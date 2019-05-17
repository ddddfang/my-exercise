#include "table.h"
#include "node_btree.h"

//fang:
//table layer should be core B-tree implement to find which row we should return

//Even if key 0 does not exist in the table, this method will return the position of the lowest id
Cursor* table_start(Table* table) {

	Cursor* cursor =  table_find(table, 0);

	void* node = pager_get_page(table->pager, cursor->page_num);
	uint32_t num_cells = *leaf_node_num_cells(node);
	cursor->end_of_table = (num_cells == 0);

	return cursor;
}

//if hit, just return it's position (user determine whether to return a error or override).
//otherwise, return a postion it should be insert.(so maybe "move op" is need next)
//NOTICE: this position must be a leaf node?
Cursor* table_find(Table* table, uint32_t key) {
	uint32_t root_page_num = table->root_page_num;
	void *root_node = pager_get_page(table->pager, root_page_num);

	if (get_node_type(root_node) == NODE_LEAF) {
		return leaf_node_find(table, root_page_num, key);
	} else {
		return internal_node_find(table, root_page_num, key);
	}
}

//given page_num & cell_num, will return addr of a row in table
void* cursor_value(Cursor* cursor) {
	void* page = pager_get_page(cursor->table->pager, cursor->page_num);
	return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor* cursor) {
	void* node = pager_get_page(cursor->table->pager, cursor->page_num);
	cursor->cell_num += 1;
	if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
		// Advance to next leaf node
		uint32_t next_page_num = *leaf_node_next_leaf(node);

		if (next_page_num == 0) {	// This was rightmost leaf
			cursor->end_of_table = true;
		} else {
			cursor->page_num = next_page_num;
			cursor->cell_num = 0;
		}
	}
}

Table* db_open(const char* filename) {
	Pager* pager = pager_open(filename);
	
	Table* table = (Table *)malloc(sizeof(Table));
	table->pager = pager;
	table->root_page_num = 0;

	if (pager->num_pages == 0) {
		// New database file. Initialize page 0 as leaf node.
		void* root_node = pager_get_page(pager, 0);
		initialize_leaf_node(root_node);
		set_node_root(root_node, true);
	}
	return table;
}

void db_close(Table* table) {
	Pager* pager = table->pager;
	//flush cache
	for (uint32_t i = 0; i < pager->num_pages; i++) {
		if (pager->pages[i] == NULL) {
			continue;
		}
		pager_flush(pager, i);	//flush to file
		free(pager->pages[i]);
		pager->pages[i] = NULL;
	}

	int result = close(pager->file_descriptor);
	if (result == -1) {
		printf("Error closing db file.\n");
		exit(EXIT_FAILURE);
	}

	//free cache
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		void* page = pager->pages[i];
		if (page) {
			free(page);
			pager->pages[i] = NULL;
		}
	}
	free(pager);
}


