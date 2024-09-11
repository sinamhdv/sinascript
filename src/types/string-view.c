#include "string-view.h"
#include "../utils/utils.h"
#include <string.h>

String *String_new(size_t size) {
	String *str = checked_malloc(sizeof(String) + size);
	str->size = size;
	memset(str->data, 0, size);
	return str;
}

int String_cmpstr(String *a, String *b) {
	return !(a->size == b->size && memcmp(a->data, b->data, a->size) == 0);
}

int String_cmparr(String *a, char *b) {
	size_t b_size = strlen(b);
	return !(a->size == b_size && memcmp(a->data, b, b_size) == 0);
}

void String_free(String *str) {
	free(str);
}