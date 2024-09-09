#include "../types/string-view.h"

#ifndef MYHEADER_TOKEN_H
#define MYHEADER_TOKEN_H

typedef enum TokenType {
	TOKEN_SINGLE = 0,	// single char token
	TOKEN_COMPARE,		// > < = >= <= ==
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

#define TOKEN_IS_CLOSING_CURLY_BRACE(t) ((t)->type == TOKEN_SINGLE && (t)->str.data[0] == '}')

#endif
