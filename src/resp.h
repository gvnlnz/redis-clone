#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUF_SIZE 512

typedef void * (*t_func_ptr)(const char *msg);

int destroy_buffer(void *buffer);
void *parse_simple_string(const char *msg);
void *parse_bulk_string(const char *msg);
void *parse_int(const char *msg);
void *parse_error(const char *msg);
void *parse_array(const char *msg);
void *parse_null(const char *msg);
void *parse_double(const char *msg);
void *dispatch(char inst, const char *msg);
void *parse_msg(const char *msg);