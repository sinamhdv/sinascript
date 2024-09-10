#include "../types/string-view.h"

#ifndef MYHEADER_TOKEN_H
#define MYHEADER_TOKEN_H

/*
double-char operators are: >= <= == !=
*/

typedef enum TokenType {
	TOKEN_OPERATOR = 0,	// operator token
	TOKEN_STRING,		// string literal
	TOKEN_NUMBER,		// number literal
	TOKEN_KEYWORD,		// keyword
	TOKEN_IDENTIFIER,	// identifier
	TOKEN_EOF,			// end of file
} TokenType;

typedef struct Token {
	struct Token *next;
	struct Token *prev;
	TokenType type;
	String str;
} Token;

Token *add_token_to_list(Token **tok_list, Token *token);
Token *Token_new(size_t str_size);

#define TOKEN_IS_OPERATOR(t, c) ((t)->type == TOKEN_OPERATOR \
	&& (t)->str.size + 1 == sizeof((c)) \
	&& (t)->str.data[0] == ((c)[0]) \
	&& ((t)->str.size == 1 || (t)->str.data[1] == (c)[1]))

#endif
