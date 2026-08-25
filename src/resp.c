#include "resp.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int resp_destroy(t_resp_info *buffer) {
	free(buffer);
	return 0;
}

int is_valid_format(const char *msg) {
	const char *cmsg = (const char *)msg;
	if(*cmsg != '\r' || *(cmsg+1) != '\n') {
        return 0;
    }
	return 1;
}

void *decode_simple_string(const char *msg) {
	t_resp_info *value = (t_resp_info *)malloc(sizeof(t_resp_info));
	if(!value) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}

	memset(value, 0, sizeof(t_resp_info));
	value->type = RESP_SIMPLE_STRING;
	
	/* now i expect the simple string */
	size_t len = 0;
	const char *start = msg;
	while (*msg != '\r') {
		len++;
		msg++;
	}
	memcpy(value->data.buffer, start, len);
	value->data.buffer[len] = '\0';

	if(!is_valid_format(msg)) {
		resp_destroy(value);
  		printf("[Error] invalid format.\n");
    	msg = start; /* backup of the pointer to the initial position. */
    	return NULL;
	}
	
	return value;
}

void *decode_bulk_string(const char *msg) {
	t_resp_info *value = (t_resp_info *)malloc(sizeof(t_resp_info));
	if(!value) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
    long len;
    char *endptr;

    memset(value, 0, sizeof(t_resp_info));
    value->type = RESP_BULK_STRING;

    /**
     * strtol stores the first invalid character in &endptr.
     * returns a 10-based long integer.
     */
    errno = 0;
    len = strtol(msg, &endptr, 10);

    /* length overflow check */
    if (errno == ERANGE) {
    	printf("[Error] length out of range.\n");
     	resp_destroy(value);
     	return NULL;
    }

    /* no digit found */
    if (endptr == msg) {
    	printf("[Error] invalid format.\n");
     	resp_destroy(value);
     	return NULL;
    }

    /* the number must be followed immediately by "\r\n" */
    if(!is_valid_format(endptr)) {
    	resp_destroy(value);
    	return NULL;
    }

    /* null bulk string */
    if (len == -1) {
    	printf("Null bulk string detected.\n");
        value->type = RESP_NULL;
        return value;
    }

    /* other negatives length are not allowed */
    if (len < 0) {
   		printf("[Error] negative length is not allowed.\n");
     	resp_destroy(value);
    	return NULL;
    }

    /* buffer overflow check */
    if(len >= MAX_BUF_SIZE) {
        printf("Bulk string length too big.\n");
        resp_destroy(value);
        return NULL;
    }

    msg = endptr + 2; // points to the start of the string

    memcpy(value->data.buffer, msg, len);
    value->data.buffer[len] = '\0';

    msg += len;

    /* string must be terminated with "\r\n" */
    if(!is_valid_format(msg)) {
    	resp_destroy(value);
    	return NULL;
    }

    return value; // return a pointer after "\r\n"
}

void *decode_int(const char *msg) {
	t_resp_info *value = (t_resp_info *)malloc(sizeof(t_resp_info));
	if(!value) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	memset(value, 0, sizeof(t_resp_info));
	value->type = RESP_INTEGER;

	char *endptr;
	long number = strtol(msg, &endptr, 10);
	if(number > INT_MAX || number < INT_MIN) {
		printf("[Error] number size exides the limits.\n");
		resp_destroy(value);
		return NULL;
	}

	if(!is_valid_format(endptr)) {
		resp_destroy(value);
		return NULL;
	}

	value->data.integer = number;
	return value;
}

void *decode_error(const char *msg) {
	t_resp_info *value = (t_resp_info *)malloc(sizeof(t_resp_info));
	if(!value) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	memset(value, 0, sizeof(t_resp_info));
	value->type = RESP_ERROR;
	
	/* now i expect the error */
	size_t len = 0;
	const char *start = msg;
	while (*msg != '\r') {
		len++;
		msg++;
	}
	memcpy(value->data.buffer, start, len);
	value->data.buffer[len] = '\0';

	if(!is_valid_format(msg)) {
		resp_destroy(value);
    	return NULL;
	}
	
	return value;
}

void *decode_array(const char *msg) {
	(void) msg;
	printf("array -> %s\n", msg);
	return NULL;
}

void *decode_double(const char *msg) {
	t_resp_info *value = (t_resp_info *)malloc(sizeof(t_resp_info));
	if(!value) {
		printf("[Error] malloc failed.\n");
		return NULL;
	}
	
	double number;
	char *endptr;
	
	memset(value, 0, sizeof(t_resp_info));
	value->type = RESP_DOUBLE;
    
    errno = 0;
    number = strtod(msg, &endptr); // reads until '\r' or something != '.'
    
    if (errno == ERANGE) {
    	printf("[Error] number out of range.\n");
    	resp_destroy(value);
     	return NULL;
    }

    if(endptr == msg) {
    	printf("[Error] invalid number.\n");
     	resp_destroy(value);
     	return NULL;
    }

    if(!is_valid_format(endptr)) {
    	resp_destroy(value);
    	return NULL;
    }

    value->data.doub = number;
    return value;
}

/* 256 is the range of an (unsinged char) */
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
    return dispatch(inst, ++msg);
}
