#include "resp.h"

int main (void) {
    char *bulk = "-\rMalloc failed.\r\n";
   	void *parsed = parse_msg(bulk);

    if(*bulk == ':') {
    	ssize_t *number = (ssize_t *)parsed;
     	printf("number = %ld\n", *number);
    } else {
    	const char *msg = (const char *)parsed;
     	printf("msg = %s\n", msg);
    }
    return 0;
}
