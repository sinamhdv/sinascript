#include "builtins.h"
#include "../utils/errors.h"
#include <stdio.h>

static void _show_value(SSValue value) {
	switch (value.type) {
		case SSVALUE_NUM:
			printf("%ld\n", (SSNumber)value.value);
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

SSValue builtin_function_call(String *func_name, SSArray *args, AstNode *error_location_node) {
	SSValue result = {.type = SSVALUE_NUM, .value = 0};
	if (String_cmparr(func_name, "show") == 0) {
		builtin_show(args);
	} else {
		fatal_runtime_error(error_location_node);
	}
	return result;
}
