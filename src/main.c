#include "resp.h"

int main (void) {
	const char *msg = "$11\r\nHELLO WORLD\r\n";
	t_resp_info *value = decode_msg(msg);

	if(value->type == RESP_BULK_STRING) {
		printf("type of value = %d\n", value->type);
		printf("value data = %s\n", value->data.buffer);
	}

	const char *number = ":1234543\r\n";
	t_resp_info *num_value = decode_msg(number);

	if(num_value->type == RESP_INTEGER) {
		printf("type of value = %d\n", num_value->type);
		printf("value data = %ld\n", num_value->data.integer);
	}
	
	const char *doub_number = ",12231.4341\r\n";
	t_resp_info *doub_value = decode_msg(doub_number);
	
	if(doub_value->type == RESP_DOUBLE) {
		printf("type of value = %d\n", doub_value->type);
		printf("value data = %lf\n", doub_value->data.doub);
	}

    return 0;
}
