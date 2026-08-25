#include "resp.h"

int print_array_elements(const t_resp_info *node) {
	if(!node || node->type != RESP_ARRAY) {
		printf("[Error] not a RESP array.\n");
		return 1;
	}

	for(size_t i = 0; i < node->data.array.size; i++) {
		const t_resp_info *element = node->data.array.elements[i];
		printf("node type = %d\n", element->type);
		switch(element->type) {
			case RESP_BULK_STRING:
				printf("%s\n", element->data.buffer);
				break;
			case RESP_SIMPLE_STRING:
				printf("%s\n", element->data.buffer);
				break;
			case RESP_ERROR:
				printf("%s\n", element->data.buffer);
				break;
			case RESP_DOUBLE:
				printf("%lf\n", element->data.doub);
				break;
			case RESP_INTEGER:
				printf("%ld\n", element->data.integer);
				break;
			case RESP_ARRAY:
				print_array_elements(element);
				break;
			case RESP_NULL:
				printf("(nil)\n");
				break;
			default:
				printf("[default]\n");
		}
	}
	return 0;
}

int main (void) {
	const char *array = "*2\r\n$5\r\nHELLO\r\n$5\r\nWORLD\r\n";
	const char *cur = array;
	t_resp_info *decode = decode_msg(&cur);

	if(!decode) {
		printf("[Error] decode failed.\n");
		return 1;
	}

	printf("decode type = %d\n", decode->type);
	printf("decode->data.array.size = %ld\n", decode->data.array.size);
	print_array_elements(decode);

	resp_destroy(decode);
    return 0;
}
