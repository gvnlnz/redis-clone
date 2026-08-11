#include "resp.h"

int main (void) {
    char *bulk = "$11\r\nHELLO WORLD\r\n";
    const char *parsed = parse_msg(bulk);
    printf("parsed = %s\n", parsed);

    destroy_buffer((void*)parsed);
    return 0;
}
