#include "ss-value.h"

#ifndef MYHEADER_SS_STRING_H
#define MYHEADER_SS_STRING_H

typedef struct SSString {
	SSHeapHeader hhdr;
	size_t size;
	char data[];
} SSString;

#endif
