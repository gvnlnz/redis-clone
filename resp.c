#include "resp.h"

enum e_conv {
    CONV_INT        = ':',
    CONV_STR        = '+',
    CONV_BULK       = '$',
    CONV_ERR        = '-',
    CONV_ARRAY      = '*',
    CONV_NULL       = '_',
    CONV_DOUBLE     = ',',
};

const char *parse_string(const char *msg) {
    (void) msg;
    printf("parse string\n");
    return 0;
}

const char *parse_bulk_string(const char *msg) {
    char buffer[MAX_BUF_SIZE];
    memset(buffer, 0, MAX_BUF_SIZE);
    msg++; // skip inst
    unsigned int len = atoi(msg);

    if(len == 0) {
        // atoi(msg) returned 0 from something which wasn't a nunmber
        if(*(msg + 1)  != '\r' || *(msg + 2) != '\n') {
            printf("[Error] invalid format.\n");
            return NULL;
        // atoi(msg) was really 0.
        } else {
            printf("Empty string detected.\n");
            return msg + 3;
        }
    }
    if(len >= MAX_BUF_SIZE) {
        printf("Bulk string length too big.\n");
        return NULL;
    }

    while(*msg && *msg != '\r') // this is due to the possibility of a multi-character length.
        msg++;

    // expecting '\r\n'
    if(*msg != '\r' || *(msg+1) != '\n') {
        printf("[Error] invalid format.\n");
        return NULL;
    }

    msg += 2; // now msg points to the start of the string
    memcpy(buffer, msg, len);
    buffer[len] = '\0';
    msg += len;

    if(*msg != '\r' || *(msg+1) != '\n') {
        printf("[Error] invalid format.\n");
        return NULL;
    }

    printf("bulk string -> %s\n", buffer);
    return msg + 2; // return a pointer after "\r\n"
}

const char *parse_int(const char *msg) {
    (void) msg;
    printf("parse int\n");
    return 0;
}

const char *parse_error(const char *msg) {
    (void) msg;
    printf("parse error");
    return 0;
}

const char *parse_array(const char *msg) {
    (void) msg;
    printf("parse array\n");
    return 0;
}

const char *parse_null(const char *msg) {
    (void) msg;
    printf("parse null\n");
    return 0;
}

const char *parse_double(const char *msg) {
    (void) msg;
    printf("parse float\n");
    return 0;
}

const t_func_ptr g_table[256] = {
    [CONV_STR]       = parse_string,
    [CONV_BULK]      = parse_bulk_string,
    [CONV_INT]       = parse_int,
    [CONV_ERR]       = parse_error,
    [CONV_ARRAY]     = parse_array,
    [CONV_NULL]      = parse_null,
    [CONV_DOUBLE]    = parse_double,
};

const char *dispatch(char inst, const char *msg) {
    t_func_ptr fn;

    fn = g_table[(unsigned char) inst];
    if(!fn) {
        printf("[Error] - g_table failed.\n");
        return NULL;
    }
    return fn(msg);
}

int parse_msg(const char *msg) {
    // 1. read the instruction
    char inst = *msg;
    if(!inst) {
        printf("[Error] invalid format.\n");
        return -1;
    }
    // 2. dispatch the msg to instr. controller
    if (dispatch(inst, msg) == NULL) return -1;

    return 0;
}