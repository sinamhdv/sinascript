#include "ss-string.h"
#include "../heap/heap.h"

SSString *SSString_new(size_t data_size) {
	return ss_alloc(sizeof(SSString) - sizeof(SSHeapHeader) + data_size);
}
