#include "heap.h"
#include "../utils/utils.h"
#include "../objects/ss-value.h"

void *ss_alloc(size_t size) {
	SSHeapHeader *hhdr = checked_malloc(sizeof(SSHeapHeader) + size);
	hhdr->refcount = 1;
	return hhdr;
}

void ss_free(void *ptr) {
	SSHeapHeader *hhdr = (SSHeapHeader *)ptr;
	hhdr->refcount--;
	if (hhdr->refcount == 0)
		free(ptr);
}
