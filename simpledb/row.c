#include "row.h"

Row_Init_Result row_init(char *id, char *username, char *email, Row *row) {

	if (id == NULL || username == NULL || email == NULL) {
		return ROW_STRING_NULL;
	}
	int id_int = atoi(id);

	if(id_int < 0) {
		return ROW_NEGATIVE_ID;
	}
	if (strlen(username) > COLUMN_USERNAME_SIZE) {
		return ROW_STRING_TOO_LONG;
	}
	if (strlen(email) > COLUMN_EMAIL_SIZE) {
		return ROW_STRING_TOO_LONG;
	}
	if(row) {
		row->id = id_int;
		strcpy(row->username, username);
		strcpy(row->email, email);
	}
	return ROW_INIT_SUCCESS;
}

uint32_t row_get_key(Row* row) {
	return row->id;
}

void row_print(Row* row) {
	printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

//serialize_row
void row_write_to_buffer(Row* srcRow, void* destBuffer) {
	memcpy(destBuffer + ID_OFFSET, &(srcRow->id), ID_SIZE);
	memcpy(destBuffer + USERNAME_OFFSET, &(srcRow->username), USERNAME_SIZE+1);
	memcpy(destBuffer + EMAIL_OFFSET, &(srcRow->email), EMAIL_SIZE+1);
}

//deserialize_row
void row_read_from_buffer(void* srcBuffer, Row* destRow) {
	memcpy(&(destRow->id), srcBuffer + ID_OFFSET, ID_SIZE);
	memcpy(&(destRow->username), srcBuffer + USERNAME_OFFSET, USERNAME_SIZE+1);
	memcpy(&(destRow->email), srcBuffer + EMAIL_OFFSET, EMAIL_SIZE+1);
}


