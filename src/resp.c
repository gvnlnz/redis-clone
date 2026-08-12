#include "resp.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

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

void *decode_simple_string(const char *msg) {
	char *buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	if (!msg || *msg != '+') {
		printf("[Error] invalid string.\n");
		destroy_buffer(buffer);
     	return NULL;
	}

	memset(buffer, 0, MAX_BUF_SIZE);
	msg++; // skip inst (+)
	
	/* now i expect the simple string */
	size_t len = 0;
	const char *start = msg;
	while (*msg != '\r') {
		len++;
		msg++;
	}
	memcpy(buffer, start, len);
	buffer[len] = '\0';

	if(!is_valid_format(msg)) {
		destroy_buffer(buffer);
    	return NULL;
	}
	
	return buffer;
}

void *decode_bulk_string(const char *msg) {
	char *buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
    long len;
    char *endptr;

    if(!msg || *msg != '$') {
    	printf("[Error] invalid string.\n");
     	destroy_buffer(buffer);
     	return NULL;
    }

    memset(buffer, 0, MAX_BUF_SIZE);
    msg++; // skip inst ($)

    /**
     * strtol stores the first invalid character in &endptr.
     * returns a 10-based long integer.
     */
    errno = 0;
    len = strtol(msg, &endptr, 10);

    /* length overflow check */
    if (errno == ERANGE) {
    	printf("[Error] length out of range.\n");
     	destroy_buffer(buffer);
     	return NULL;
    }

    /* no digit found */
    if (endptr == msg) {
    	printf("[Error] invalid format.\n");
     	destroy_buffer(buffer);
     	return NULL;
    }

    /* the number must be followed immediately by "\r\n" */
    if(!is_valid_format(endptr)) {
    	destroy_buffer(buffer);
    	return NULL;
    }

    /* null bulk string */
    if (len == -1) {
    	printf("Null bulk string detected.\n");
     	destroy_buffer(buffer);
     	return endptr + 2;
    }

    /* other negatives length are not allowed */
    if (len < 0) {
   		printf("[Error] negative length is not allowed.\n");
     	destroy_buffer(buffer);
    	return NULL;
    }

    /* buffer overflow check */
    if(len >= MAX_BUF_SIZE) {
        printf("Bulk string length too big.\n");
        destroy_buffer(buffer);
        return NULL;
    }

    msg = endptr + 2; // points to the start of the string

    memcpy(buffer, msg, len);
    buffer[len] = '\0';

    msg += len;

    /* string must be terminated with "\r\n" */
    if(!is_valid_format(msg)) {
    	destroy_buffer(buffer);
    	return NULL;
    }

    return buffer; // return a pointer after "\r\n"
}

void *decode_int(const char *msg) {
	ssize_t *buffer = malloc(sizeof(ssize_t));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}

	if (!msg || *msg != ':') {
		printf("[Error] invalid format.\n");
		destroy_buffer(buffer);
		return NULL;
	}

	memset(buffer, 0, sizeof(ssize_t));
	msg++; // skip inst (:)

	char *endptr;
	long number = strtol(msg, &endptr, 10);
	if(number > INT_MAX || number < INT_MIN) {
		printf("[Error] number size exides the limits.\n");
		destroy_buffer(buffer);
		return NULL;
	}

	printf("number -> %ld\n", number);
	if(!is_valid_format(endptr)) {
		destroy_buffer(buffer);
		return NULL;
	}

	memcpy(buffer, &number, sizeof(ssize_t));
	return buffer;
}

void *decode_error(const char *msg) {
	char *buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	if (!msg || *msg != '-') {
		printf("[Error] invalid format.\n");
		destroy_buffer(buffer);
     	return NULL;
	}

	memset(buffer, 0, MAX_BUF_SIZE);
	msg++; // skip inst (-)
	
	/* now i expect the error */
	size_t len = 0;
	const char *start = msg;
	while (*msg != '\r') {
		len++;
		msg++;
	}
	memcpy(buffer, start, len);
	buffer[len] = '\0';

	if(!is_valid_format(msg)) {
		destroy_buffer(buffer);
    	return NULL;
	}
	
	return buffer;
}

void *decode_array(const char *msg) {
    (void) msg;
    printf("parse array\n");
    return 0;
}

void *decode_double(const char *msg) {
	double *buffer, number;
	char *endptr;
	
	buffer = (double *)malloc(sizeof(double));
	if(!buffer) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	memset(buffer, 0, sizeof(double));
    
    msg++; // skip inst (,)
    errno = 0;
    number = strtod(msg, &endptr); // reads unitl '\r' or something != '.'
    
    if (errno == ERANGE) {
    	printf("[Error] number out of range.\n");
    	destroy_buffer(buffer);
     	return NULL;
    }

    if(endptr == msg) {
    	printf("[Error] invalid number.\n");
     	destroy_buffer(buffer);
     	return NULL;
    }

    if(!is_valid_format(endptr)) {
    	destroy_buffer(buffer);
    	return NULL;
    }

    *buffer = number;
    return buffer;
}

const t_func_ptr g_table[256] = {
    [CONV_STR]       = decode_simple_string,
    [CONV_BULK]      = decode_bulk_string,
    [CONV_INT]       = decode_int,
    [CONV_ERR]       = decode_error,
    [CONV_ARRAY]     = decode_array,
    [CONV_DOUBLE]    = decode_double,
};

void *dispatch(char inst, const char *msg) {
    t_func_ptr fn;

    fn = g_table[(unsigned char) inst];
    if(!fn) return NULL;

    return fn(msg);
}

void *decode_msg(const char *msg) {
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
