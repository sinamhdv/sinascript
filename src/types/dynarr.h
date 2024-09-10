#include <stdlib.h>

#ifndef MYHEADER_DYNARR_H
#define MYHEADER_DYNARR_H

typedef struct DynArr {
	size_t size;
	size_t capacity;
	void **arr;
} DynArr;

void DynArr_init(DynArr *dynarr, size_t init_size);
void DynArr_push(DynArr *dynarr, void *value);
void DynArr_free(DynArr *dynarr);
void DynArr_resize(DynArr *dynarr, size_t new_capacity);

#endif
