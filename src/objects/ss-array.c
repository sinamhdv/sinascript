#include "ss-array.h"
#include "../heap/heap.h"
#include <string.h>

SSArray *SSArray_new(size_t elem_count) {
	SSArray *arr = ss_alloc(sizeof(SSArray) - sizeof(SSHeapHeader) + sizeof(SSValue) * elem_count);
	arr->size = elem_count;
	return arr;
}

SSArray *SSArray_concat(SSArray *a, SSArray *b) {
	SSArray *result = SSArray_new(a->size + b->size);
	memcpy(result->data, a->data, a->size * sizeof(SSValue));
	memcpy(result->data + a->size, b->data, b->size * sizeof(SSValue));
	return result;
}

int SSArray_cmp(SSArray *a, SSArray *b) {
	if (a->size != b->size) return 1;
	for (size_t i = 0; i < a->size; i++) {
		if (SSValue_cmp(a->data[i], b->data[i]) != 0)
			return 1;
	}
	return 0;
}
