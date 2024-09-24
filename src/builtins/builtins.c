#include "builtins.h"
#include "../utils/errors.h"
#include <stdio.h>

static void _show_value(SSValue value) {
	switch (value.type) {
		case SSVALUE_NUM:
			printf("%ld", (SSNumber)value.value);
			break;
		case SSVALUE_STR: {
			SSString *str = (SSString *)value.value;
			fwrite(str->data, 1, str->size, stdout);
			break;
		}
		case SSVALUE_ARR: {
			SSArray *arr = (SSArray *)value.value;
			putchar('[');
			for (size_t i = 0; i < arr->size; i++) {
				_show_value(arr->data[i]);
				if (i + 1 != arr->size) {
					putchar(',');
					putchar(' ');
				}
			}
			putchar(']');
			break;
		}
	}
}

static void builtin_show(SSArray *args) {
	for (size_t i = 0; i < args->size; i++) {
		_show_value(args->data[i]);
		if (i + 1 != args->size)
			putchar(' ');
	}
}

static void builtin_alert(SSArray *args) {
	if (args->size != 1) fatal_runtime_error(NULL);
	_show_value(args->data[0]);
	getchar();
}

static SSValue builtin_length(SSArray *args) {
	if (args->size != 1) fatal_runtime_error(NULL);
	SSValue arg = args->data[0];
	switch (arg.type) {
		case SSVALUE_ARR:
			return (SSValue){.type = SSVALUE_NUM, .value = (void*)((SSArray *)arg.value)->size};
		case SSVALUE_STR:
			return (SSValue){.type = SSVALUE_NUM, .value = (void*)((SSString *)arg.value)->size};
		default:
			fatal_runtime_error(NULL);
	}
}

#ifdef DEBUG
static void builtin_debugprint(SSArray *args) {
	if (args->size != 1) fatal_runtime_error(NULL);
	SSValue val = args->data[0];
	printf("type: %d, value: %p\n", val.type, val.value);
}
#endif

SSValue builtin_function_call(String *func_name, SSArray *args, AstNode *error_location_node) {
	SSValue result = {.type = SSVALUE_NUM, .value = 0};
	if (String_cmparr(func_name, "show") == 0) {
		builtin_show(args);
	} else if (String_cmparr(func_name, "alert") == 0) {
		builtin_alert(args);
	} else if (String_cmparr(func_name, "length") == 0) {
		return builtin_length(args);
	}
#ifdef DEBUG
	else if (String_cmparr(func_name, "debugprint") == 0) {
		builtin_debugprint(args);
	}
#endif
	else {
		fatal_runtime_error(error_location_node);
	}
	return result;
}
