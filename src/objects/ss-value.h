#include "ss-number.h"

#ifndef MYHEADER_SS_VALUE_H
#define MYHEADER_SS_VALUE_H

typedef enum SSValueType {
	SSVALUE_NUM,
	SSVALUE_STR,
	SSVALUE_ARR,
} SSValueType;

typedef struct SSValue {
	SSValueType type;
	void *value;
} SSValue;

#define SSVALUE_TRUTH_VAL(v) (((v).type != SSVALUE_NUM || (SSNumber)(v).value != 0) ? 1 : 0)

#endif
