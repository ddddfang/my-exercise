#ifndef __INPUTS_H
#define __INPUTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct InputBuffer_t{
	char *buffer;
	size_t buffer_length;
	ssize_t input_length;
} InputBuffer;

InputBuffer *new_input_buffer();
void read_input(InputBuffer *input_buffer);

#endif
