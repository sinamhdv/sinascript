#include <stdlib.h>

#ifndef MYHEADER_STRING_H
#define MYHEADER_STRING_H

typedef struct String {
	size_t size;
	char data[];
} String;

String *String_new(size_t size);
void String_free(String *str);

#endif