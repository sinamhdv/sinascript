#include "string.h"
#include <string.h>

String *String_new(size_t size) {
	String *str = malloc(sizeof(String) + size);
	if (str == NULL) return NULL;
	str->size = 0;
	memset(str->data, 0, size);
	return str;
}

void String_free(String *str) {
	free(str);
}