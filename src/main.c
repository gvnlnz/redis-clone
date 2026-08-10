#include "resp.h"

int main (void) {
    char *bulk = "(5\r\nhello\r\n";
    parse_msg(bulk);
    return 0;
}
