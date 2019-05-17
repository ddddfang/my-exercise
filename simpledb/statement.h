#ifndef __STATEMENT_H
#define __STATEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "inputs.h"
#include "table.h"
#include "row.h"

typedef enum PrepareResult_t {
	PREPARE_SUCCESS,
	PREPARE_SYNTAX_ERROR,
	PREPARE_STRING_TOO_LONG,
	PREPARE_NEGATIVE_ID,
	PREPARE_UNRECOGNIZED
} PrepareResult;

typedef enum StatementType_t {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

typedef struct Statement_t {
	StatementType type;
	Row row_insert;
} Statement;

typedef enum ExecuteResult_t {
	EXECUTE_SUCCESS,
	EXECUTE_DUPLICATE_KEY,
	EXECUTE_TABLE_FULL
} ExecuteResult;


PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);
ExecuteResult execute_statement(Statement* statement, Table* table);


#endif

