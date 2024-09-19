#include "heap.h"
#include "../utils/utils.h"
#include "../objects/ss-value.h"
#include "../objects/ss-array.h"
#include <pthread.h>

extern pthread_mutex_t async_mutex;

void *ss_alloc(size_t size) {
	SSHeapHeader *hhdr = checked_malloc(sizeof(SSHeapHeader) + size);
	hhdr->refcount = 0;
	return hhdr;
}

void ss_free_if_noref(void *ptr, int recursive) {
	SSHeapHeader *hhdr = (SSHeapHeader *)ptr;
	if (hhdr->refcount == 0) {
		if (recursive) {
			SSArray *arr = (SSArray *)ptr;
			for (size_t i = 0; i < arr->size; i++) {
				ss_value_dec_refcount(arr->data[i]);
			}
		}
		free(ptr);
	}
}

void ss_dec_refcount(void *ptr, int recursive) {
	SSHeapHeader *hhdr = (SSHeapHeader *)ptr;
	hhdr->refcount--;
	ss_free_if_noref(ptr, recursive);
}

void ss_inc_refcount(void *ptr) {
	SSHeapHeader *hhdr = (SSHeapHeader *)ptr;
	hhdr->refcount++;
}

void ss_value_free_if_noref(SSValue value) {
	pthread_mutex_lock(&async_mutex);
	if ((int)value.type >= 0)
		ss_free_if_noref(value.value, value.type == SSVALUE_ARR);
	pthread_mutex_unlock(&async_mutex);
}

void ss_value_dec_refcount(SSValue value) {
	pthread_mutex_lock(&async_mutex);
	if ((int)value.type >= 0)
		ss_dec_refcount(value.value, value.type == SSVALUE_ARR);
	pthread_mutex_unlock(&async_mutex);
}

void ss_value_inc_refcount(SSValue value) {
	pthread_mutex_lock(&async_mutex);
	if ((int)value.type >= 0)
		ss_inc_refcount(value.value);
	pthread_mutex_unlock(&async_mutex);
}

// void ss_immediately_free(void *ptr, int recursive) {
// 	if (recursive) {
// 		SSArray *arr = (SSArray *)ptr;
// 		for (size_t i = 0; i < arr->size; i++) {
// 			ss_value_immediately_free(arr->data[i]);
// 		}
// 	}
// 	free(ptr);
// }

// void ss_value_immediately_free(SSValue value) {
// 	if ((int)value.type >= 0)
// 		ss_immediately_free(value.value, value.type == SSVALUE_ARR);
// }
