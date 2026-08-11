#include "resp.h"
#include <errno.h>

enum e_conv {
    CONV_INT        = ':',
    CONV_STR        = '+',
    CONV_BULK       = '$',
    CONV_ERR        = '-',
    CONV_ARRAY      = '*',
    CONV_NULL       = '_',
    CONV_DOUBLE     = ',',
};

int destroy_buffer(void *buffer) {
	free(buffer);
	return 0;
}

int is_valid_format(const char *msg) {
	const char *cmsg = (const char *)msg;
	if(*cmsg != '\r' || *(cmsg+1) != '\n') {
        printf("[Error] invalid format.\n");
        return 0;
    }
	return 1;
}

const char *parse_simple_string(const char *msg) {
	char *buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	if (!msg || *msg != '+') {
		printf("[Error] invalid string.\n");
     	return NULL;
	}

	printf("*msg -> %c\n", *msg);
	memset(buffer, 0, MAX_BUF_SIZE);
	msg++;
	printf("*msg -> %c\n", *msg);
	
	/* now i expect the simple string */
	size_t len = 0;
	const char *start = msg;
	while (*msg != '\r') {
		len++;
		msg++;
	}
	memcpy(buffer, start, len);
	buffer[len] = '\0';

	if(!is_valid_format(msg))
    	return NULL;
	
	return buffer;
}

const char *parse_bulk_string(const char *msg) {
	char *buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
    long len;
    char *endptr;

    if(!msg || *msg != '$') {
    	printf("[Error] invalid string.\n");
     	return NULL;
    }

    memset(buffer, 0, MAX_BUF_SIZE);
    msg++; // skip inst

    /**
     * strtol stores the first invalid character in &endptr.
     * returns a 10-based long integer.
     */
    errno = 0;
    len = strtol(msg, &endptr, 10);

    /* length overflow check */
    if (errno == ERANGE) {
    	printf("[Error] length out of range.\n");
     	return NULL;
    }

    /* no digit found */
    if (endptr == msg) {
    	printf("[Error] invalid format.\n");
     	return NULL;
    }

    /* the number must be followed immediately by "\r\n" */
    if(!is_valid_format(endptr))
    	return NULL;

    /* null bulk string */
    if (len == -1) {
    	printf("Null bulk string detected.\n");
     	return endptr + 2;
    }

    /* other negatives length are not allowed */
    if (len < 0) {
   		printf("[Error] negative length is not allowed.\n");
    	return NULL;
    }

    /* buffer overflow check */
    if(len >= MAX_BUF_SIZE) {
        printf("Bulk string length too big.\n");
        return NULL;
    }

    msg = endptr + 2; // points to the start of the string

    memcpy(buffer, msg, len);
    buffer[len] = '\0';

    msg += len;

    /* string must be terminated with "\r\n" */
    if(!is_valid_format(msg))
    	return NULL;

    return buffer; // return a pointer after "\r\n"
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
    [CONV_STR]       = parse_simple_string,
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
    if(!fn) return NULL;

    return fn(msg);
}

const char *parse_msg(const char *msg) {
    // 1. read the instruction
    char inst;

    if(!msg || !*msg) {
    	printf("[Error] invalid format.\n");
     	return NULL;
    }

    inst = *msg;

    if(!g_table[(unsigned char) inst]) {
    	printf("[Error] bad instruction.\n");
     	return NULL;
    }

    // 2. dispatch the msg to instr. controller
    return dispatch(inst, msg);
}
