#include "string.h"
#include <string.h>
#include "utils.h"

String *String_new(size_t size) {
	String *str = checked_malloc(sizeof(String) + size);
	str->size = size;
	memset(str->data, 0, size);
	return str;
}

void String_free(String *str) {
	free(str);
}