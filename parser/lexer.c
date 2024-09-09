#include "lexer.h"
#include <ctype.h>

Token *tokenize_source(String *source) {
	size_t i = 0;
	while (i < source->size) {
		char c = source->data[i];
		Token token;
		if (isdigit(c)) {
			token = tokenize_num(source, i)
		}
	}
}
