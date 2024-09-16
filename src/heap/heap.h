#include "../objects/ss-value.h"
#include <stdlib.h>

#ifndef MYHEADER_HEAP_H
#define MYHEADER_HEAP_H

void *ss_alloc(size_t size);
void ss_free_if_noref(void *ptr, int recursive);
void ss_dec_refcount(void *ptr, int recursive);
void ss_inc_refcount(void *ptr);
void ss_value_free_if_noref(SSValue value);
void ss_value_dec_refcount(SSValue value);

#endif
