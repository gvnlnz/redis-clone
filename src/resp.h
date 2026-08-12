#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUF_SIZE 512
typedef void * (*t_func_ptr)(const char *msg);

void *decode_simple_string(const char *msg);
void *dispatch(char inst, const char *msg);
void *decode_bulk_string(const char *msg);
void *decode_double(const char *msg);
void *decode_array(const char *msg);
void *decode_error(const char *msg);
void *decode_null(const char *msg);
void *decode_msg(const char *msg);
void *decode_int(const char *msg);
int destroy_buffer(void *buffer);