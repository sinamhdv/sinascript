#include "ss-string.h"
#include "../heap/heap.h"

SSString *SSString_new(size_t data_size) {
	SSString *str = ss_alloc(sizeof(SSString) - sizeof(SSHeapHeader) + data_size);
	str->size = data_size;
	return str;
}
