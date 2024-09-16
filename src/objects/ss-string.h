#include "ss-value.h"

#ifndef MYHEADER_SS_STRING_H
#define MYHEADER_SS_STRING_H

typedef struct SSString {
	SSHeapHeader hhdr;
	size_t size;
	char data[];
} SSString;

SSString *SSString_new(size_t data_size);
SSString *SSString_concat(SSString *a, SSString *b);
int SSString_cmp(SSString *a, SSString *b);

#endif
