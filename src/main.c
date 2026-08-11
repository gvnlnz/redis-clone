#include "resp.h"

int main (void) {
    char *bulk = ":-1234567876\r\n";
   	void *parsed = parse_msg(bulk);

    if(*bulk == ':') {
    	ssize_t *number = (ssize_t *)parsed;
     	printf("number = %ld\n", *number);
    }
    return 0;
}
