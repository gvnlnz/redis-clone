#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUF_SIZE 512

typedef const char * (*t_func_ptr)(const char *msg);

int destroy_buffer(void *buffer);
const char *parse_simple_string(const char *msg);
const char *parse_bulk_string(const char *msg);
const char *parse_int(const char *msg);
const char *parse_error(const char *msg);
const char *parse_array(const char *msg);
const char *parse_null(const char *msg);
const char *parse_double(const char *msg);

const char *dispatch(char inst, const char *msg);
const char *parse_msg(const char *msg);