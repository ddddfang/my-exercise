#ifndef __ROW_H
#define __ROW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 32

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define ID_SIZE			size_of_attribute(Row, id)
#define USERNAME_SIZE	size_of_attribute(Row, username)
#define EMAIL_SIZE		size_of_attribute(Row, email)
#define ID_OFFSET			0
#define USERNAME_OFFSET		(ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET		(USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE	(ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)


typedef struct Row_t{
	uint32_t id;
	char username[COLUMN_USERNAME_SIZE+1];
	char email[COLUMN_EMAIL_SIZE+1];
} Row;

typedef enum Row_Init_Result_t{
	ROW_INIT_SUCCESS,
	ROW_NEGATIVE_ID,
	ROW_STRING_NULL,
	ROW_STRING_TOO_LONG
} Row_Init_Result;


Row_Init_Result row_init(char *id, char *username, char *email, Row *row);
uint32_t row_get_key(Row* row);
void row_write_to_buffer(Row* srcRow, void* destBuffer); //serialize_row
void row_read_from_buffer(void* srcBuffer, Row* destRow); //deserialize_row
void row_print(Row* row);

#endif

