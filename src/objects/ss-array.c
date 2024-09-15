#include "ss-array.h"
#include "../heap/heap.h"

SSArray *SSArray_new(size_t elem_count) {
	SSArray *arr = ss_alloc(sizeof(SSArray) - sizeof(SSHeapHeader) + sizeof(SSValue) * elem_count);
	arr->size = elem_count;
	return arr;
}
