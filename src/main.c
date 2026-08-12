#include "resp.h"

int main (void) {
    char *bulk = ",14714814.11211318931\r\n";
   	void *parsed = decode_msg(bulk);

    if(*bulk == ':') {
    	ssize_t *number = (ssize_t *)parsed;
     	printf("number = %ld\n", *number);
    } else if (*bulk == ',') {
    	double *number = (double *)parsed;
     	printf("number = %lf\n", *number);
    } else {
    	const char *msg = (const char *)parsed;
     	printf("msg = %s\n", msg);
    }
    return 0;
}
