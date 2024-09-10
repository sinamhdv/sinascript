#include "dynarr.h"
#include "../utils/utils.h"

void DynArr_init(DynArr *dynarr, size_t init_size) {
	dynarr->size = dynarr->capacity = init_size;
	dynarr->arr = (init_size == 0 ? NULL : checked_malloc(init_size * sizeof(void *)));
}

void DynArr_resize(DynArr *dynarr, size_t new_capacity) {
	dynarr->arr = checked_realloc(dynarr->arr, new_capacity * sizeof(void *));
	dynarr->capacity = new_capacity;
	if (dynarr->size > new_capacity)
		dynarr->size = new_capacity;
}

void DynArr_push(DynArr *dynarr, void *value) {
	if (dynarr->size == dynarr->capacity) {
		size_t new_capacity = 2 * dynarr->capacity;
		if (new_capacity == 0) new_capacity = 1;
		DynArr_resize(dynarr, new_capacity);
	}
	dynarr->arr[dynarr->size++] = value;
}

void DynArr_free(DynArr *dynarr) {
	free(dynarr->arr);
	dynarr->arr = NULL;
	dynarr->size = dynarr->capacity = 0;
}
