#include <stdlib.h>

#ifndef MYHEADER_STRING_H
#define MYHEADER_STRING_H

typedef struct String {
	size_t size;
	char data[];
} String;

String *String_new(size_t size);
void String_free(String *str);

// return 0 if a and b are the same. Non-zero if different
int String_cmpstr(String *a, String *b);
int String_cmparr(String *a, char *b);

#endif