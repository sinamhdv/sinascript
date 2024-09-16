#include "ss-string.h"
#include "../heap/heap.h"
#include <string.h>

SSString *SSString_new(size_t data_size) {
	SSString *str = ss_alloc(sizeof(SSString) - sizeof(SSHeapHeader) + data_size);
	str->size = data_size;
	return str;
}

SSString *SSString_concat(SSString *a, SSString *b) {
	SSString *result = SSString_new(a->size + b->size);
	memcpy(result->data, a->data, a->size);
	memcpy(result->data + a->size, b->data, b->size);
	return result;
}

int SSString_cmp(SSString *a, SSString *b) {
	if (a->size != b->size) return 1;
	return memcmp(a->data, b->data, a->size);
}
