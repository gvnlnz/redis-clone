#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUF_SIZE 512

typedef void * (*t_func_ptr)(const char *msg);

typedef enum {
    RESP_SIMPLE_STRING,
    RESP_BULK_STRING,
    RESP_INTEGER,
    RESP_ARRAY,
    RESP_ERROR,
    RESP_NULL,
    RESP_DOUBLE,
} t_resp_type;

enum e_conv {
    CONV_INT        = ':',
    CONV_STR        = '+',
    CONV_BULK       = '$',
    CONV_ERR        = '-',
    CONV_ARRAY      = '*',
    CONV_NULL       = '_',
    CONV_DOUBLE     = ',',
};

typedef struct resp_array {
	size_t size;
	struct resp_value **elements;
} resp_array;

typedef struct resp_value {
	t_resp_type type;

	/* only need one type of the following */
	union {	
		char *string;
		long integer;
		double doub;
		resp_array array;
	} data;
} resp_value;

void *decode_simple_string(const char *msg);
void *dispatch(char inst, const char *msg);
void *decode_bulk_string(const char *msg);
void *decode_double(const char *msg);
void *decode_array(const char *msg);
void *decode_error(const char *msg);
void *decode_msg(const char *msg);
void *decode_int(const char *msg);
int destroy_buffer(void *buffer);