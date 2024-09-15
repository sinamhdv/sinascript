#include "ss-value.h"

#ifndef MYHEADER_SS_ARRAY_H
#define MYHEADER_SS_ARRAY_H

typedef struct SSArray {
	SSHeapHeader hhdr;
	size_t size;
	SSValue data[];
} SSArray;

SSArray *SSArray_new(size_t elem_count);

#endif
