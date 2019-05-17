#include "statement.h"
#include "node_btree.h"

//fang:
//this layer is sql command processor (syntax paser, parse string and decide which method to lanuch)
//virtual machine executer.

static PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
	statement->type = STATEMENT_INSERT;

	char* keyword = strtok(input_buffer->buffer, " ");	//use " " to divide this string,get the no.1 ("insert")
	char* id_string = strtok(NULL, " ");				//get the no.2 (<id>)
	char* username = strtok(NULL, " ");					//get the no.3 (<username>)
	char* email = strtok(NULL, " ");					//get the no.4 (<email>)

	switch(row_init(id_string,username,email,&(statement->row_insert))) {
		case ROW_INIT_SUCCESS:
			return PREPARE_SUCCESS;
		case ROW_NEGATIVE_ID:
			return PREPARE_NEGATIVE_ID;
		case ROW_STRING_NULL:
			return PREPARE_SYNTAX_ERROR;
		case ROW_STRING_TOO_LONG:
			return PREPARE_STRING_TOO_LONG;
	}
	return PREPARE_SUCCESS;
}

static ExecuteResult execute_insert(Statement* statement, Table* table) {
	Row* row_to_insert = &(statement->row_insert);
	uint32_t key_to_insert = row_get_key(row_to_insert);

	//find the leaf position we should insert
	Cursor* cur = table_find(table, key_to_insert);	//todo: is the node position we finally found a LEAF node ??
	void* node = pager_get_page(table->pager, cur->page_num);
	uint32_t num_cells = *leaf_node_num_cells(node);		//todo ? this has no sense if cur pointer to a internal node.
	if (cur->cell_num < num_cells) {	//this means there is a posibility that we already have this key(DUPLICATE_KEY). we should check.
		uint32_t key_at_index = *leaf_node_key(node, cur->cell_num);
		if (key_at_index == key_to_insert) {
			return EXECUTE_DUPLICATE_KEY;
		}
	}

	leaf_node_insert(cur, key_to_insert, row_to_insert);
	free(cur);
	return EXECUTE_SUCCESS;
}

static ExecuteResult execute_select(Statement* statement, Table* table) {
	Row row;
	Cursor *cur = table_start(table);
	while(!cur->end_of_table) {
		row_read_from_buffer(cursor_value(cur), &row);
		row_print(&row);
		cursor_advance(cur);
	}
	free(cur);
	return EXECUTE_SUCCESS;
}



/******************************* interfaces **************************************/

//syntax check, and prepare statement to exec
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement) {
	if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
		statement->type = STATEMENT_INSERT;
		return prepare_insert(input_buffer,statement);
	}
	if (strcmp(input_buffer->buffer, "select") == 0) {
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}
	return PREPARE_UNRECOGNIZED;
}

//we do db statement in this func
ExecuteResult execute_statement(Statement* statement, Table* table) {
	switch (statement->type) {
		case STATEMENT_INSERT:
			return execute_insert(statement,table);
		case STATEMENT_SELECT:
			return execute_select(statement,table);
	}
	return EXECUTE_SUCCESS;
}

