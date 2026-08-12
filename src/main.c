#include "resp.h"

int main (void) {
	const char *msg = "$11\r\nHELLO WORLD\r\n";
	t_resp_info *value = decode_msg(msg);
	
	printf("type of value = %d\n", value->type);
	printf("data value = %s\n", value->data.buffer);
    return 0;
}
