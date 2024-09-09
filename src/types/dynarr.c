#include "dynarr.h"
#include "../utils/utils.h"

void DynArr_init(DynArr *dynarr, size_t init_size) {
	dynarr->size = dynarr->capacity = init_size;
	dynarr->arr = (init_size == 0 ? NULL : checked_malloc(init_size * sizeof(void *)));
}

void DynArr_resize(DynArr *dynarr, size_t size) {
	// dynarr->arr = checked_realloc()
}

void DynArr_push(DynArr *dynarr, void *value) {
	if (dynarr->size == dynarr->capacity) {
		DynArr_resize(dynarr, 2 * dynarr->capacity);
	}
}
