#include <stdio.h>

enum e_conv {
    CONV_INT        = ':',
    CONV_STR        = '+',
    CONV_BULK       = '$',
    CONV_ERR        = '-',
    CONV_ARRAY      = '*',
    CONV_NULL       = '_',
    CONV_DOUBLE     = ',',
};

int parse_string(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse string\n");
    printf("%c\n", inst);
    return 0;
}
int parse_bulk_string(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse bulk\n");
    printf("%c\n", inst);
    return 0;
}
int parse_int(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse int\n");
    printf("%c\n", inst);
    return 0;
}
int parse_error(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse error");
    printf("%c\n", inst);
    return 0;
}
int parse_array(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse array\n");
    printf("%c\n", inst);
    return 0;
}
int parse_null(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse null\n");
    printf("%c\n", inst);
    return 0;
}
int parse_double(const char inst, const char *msg) {
    (void) inst; (void) msg;
    printf("parse float\n");
    printf("%c\n", inst);
    return 0;
}

typedef int (*t_func_ptr)(const char inst, const char *msg);

const t_func_ptr g_table[256] = {
    [CONV_STR]       = parse_string,
    [CONV_BULK]      = parse_bulk_string,
    [CONV_INT]       = parse_int,
    [CONV_ERR]       = parse_error, 
    [CONV_ARRAY]     = parse_array, 
    [CONV_NULL]      = parse_null, 
    [CONV_DOUBLE]    = parse_double, 
};

int dispatch(const char inst, const char *msg) {
    t_func_ptr fn; 

    fn = g_table[(unsigned char) inst];
    if(!fn) {
        printf("[Error] - g_table failed.\n");
        return -1;
    }
    return fn(inst, msg);
}

int parse_msg(const char *msg) {
    // 1. read the instruction
    char inst = *msg;
    if(!inst) {
        printf("[Error] - invalid command.\n");
        return -1;
    }
    // 2. dispatch the msg to instr. controller
    if (dispatch(inst, msg) == -1) return -1;

    return 0;
}

int main (void) {
    char *bulk = "$5\r\nhello\r\n";

    int res = parse_msg(bulk);
    return 0;
}
