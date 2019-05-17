#include "inputs.h"

//fang:
//this layer process the input stuff get from user interface

InputBuffer *new_input_buffer() {
	InputBuffer *p = (InputBuffer *)malloc(sizeof(InputBuffer));
	p->buffer = NULL;
	p->buffer_length = 0;
	p->input_length = 0;
	return p;
}

void read_input(InputBuffer *input_buffer) {
	//getline() will do malloc work if param1 is null (and in this case param2 should be 0).
	ssize_t bytes_read = getline(&(input_buffer->buffer),&(input_buffer->buffer_length),stdin);
	if(bytes_read <= 0){
		printf("Error: error reading input.\n");
		exit(EXIT_FAILURE);
	}
	input_buffer->input_length = bytes_read - 1;	//ignore newline symbol
	input_buffer->buffer[bytes_read-1] = 0;
}



