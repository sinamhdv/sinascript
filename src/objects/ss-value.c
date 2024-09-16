#include "ss-value.h"
#include "ss-string.h"
#include "ss-array.h"
#include "../utils/errors.h"

int SSValue_cmp(SSValue a, SSValue b) {
	if (a.type != b.type) return 1;
	switch (a.type) {
		case SSVALUE_NUM:
			return (SSNumber)a.value != (SSNumber)b.value;
		case SSVALUE_STR:
			return SSString_cmp(a.value, b.value);
		case SSVALUE_ARR:
			return SSArray_cmp(a.value, b.value);
	}
	return 0;
}

SSValue SSValue_add(SSValue lval, SSValue rval, AstNode *error_location_node) {
	if (lval.type != rval.type)
		fatal_runtime_error(error_location_node);
	SSValue result = {.type = lval.type};
	switch (lval.type) {
		case SSVALUE_NUM:
			result.value = (void*)((SSNumber)lval.value + (SSNumber)rval.value);
			break;
		case SSVALUE_STR:
			result.value = SSString_concat(lval.value, rval.value);
			break;
		case SSVALUE_ARR:
			result.value = SSArray_concat(lval.value, rval.value);
			break;
		default: fatal_runtime_error(error_location_node);
	}
	return result;
}
