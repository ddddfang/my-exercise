#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "inputs.h"
#include "meta.h"
#include "table.h"
#include "statement.h"

void print_prompt() {
	printf("db> ");
}

int main(int argc, char const *argv[])
{
	if (argc < 2) {
		printf("Error: Must supply a database filename.\n");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to simple db.\n");
	InputBuffer *input_buffer = new_input_buffer();
	char const *db_file_name = argv[1];
	Table *table = db_open(db_file_name);

	while(true){
		print_prompt();
		read_input(input_buffer);
		if(is_meta_command(input_buffer)) {
			switch (do_meta_command(input_buffer,table)) {
				case META_COMMAND_SUCCESS:
					continue;
				case META_COMMAND_UNRECOGNIZED:
					printf("Error: Unrecognized meta command \"%s\".\n",input_buffer->buffer );
					continue;
			}
		}

		Statement statement;
		switch(prepare_statement(input_buffer,&statement)) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_STRING_TOO_LONG:
				printf("Error: String is too long.\n");
				continue;
			case PREPARE_NEGATIVE_ID:
				printf("Error: Id should not be negative.\n");
				continue;
			case PREPARE_SYNTAX_ERROR:
				printf("Error: Syntax error in statement \"%s\".\n",input_buffer->buffer);
				continue;
			case PREPARE_UNRECOGNIZED:
				printf("Error: Unrecognized statement \"%s\".\n",input_buffer->buffer);
				continue;
		}

		switch(execute_statement(&statement,table)){
			case EXECUTE_SUCCESS:
				printf("Executed.\n");
				break;
			case EXECUTE_DUPLICATE_KEY:
				printf("Error: Duplicate key.\n");
				break;
			case EXECUTE_TABLE_FULL:
				printf("Error: Table full.\n");
				break;
		}
	}
	return 0;
}
