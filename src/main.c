#include "resp.h"

int main (void) {
    char *bulk = "$9999999999999999999999999999999\r\n";
    const char *newptr = parse_msg(bulk);
    
    printf("newptr -> ");
    while (newptr != NULL) {
    	printf("%d", *newptr);
     	newptr++;
    }
    printf("\n");
    return 0;
}
