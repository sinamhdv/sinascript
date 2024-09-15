#include "ss-array.h"
#include "../heap/heap.h"

SSArray *SSArray_new(size_t elem_count) {
	return ss_alloc(sizeof(SSArray) - sizeof(SSHeapHeader) + sizeof(SSValue) * elem_count);
}
