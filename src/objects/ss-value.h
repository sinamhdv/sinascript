#include "ss-number.h"
#include <stdlib.h>

#ifndef MYHEADER_SS_VALUE_H
#define MYHEADER_SS_VALUE_H

typedef enum SSValueType {
	SSVALUE_NUM = -1,
	SSVALUE_STR = 0,
	SSVALUE_ARR = 1,
} SSValueType;

typedef struct SSValue {
	SSValueType type;
	void *value;
} SSValue;

typedef struct SSHeapHeader {
	size_t refcount;
} SSHeapHeader;

#define SSVALUE_TRUTH_VAL(v) (((v).type != SSVALUE_NUM || (SSNumber)(v).value != 0) ? 1 : 0)

#endif
