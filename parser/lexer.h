#include "../utils/string.h"

#ifndef MYHEADER_LEXER_H
#define MYHEADER_LEXER_H

typedef enum TokenType {
	TOKEN_NUM = 0,	// numeric literal
} TokenType;

typedef struct Token {
	TokenType type;
	struct Token *next;
	struct Token *prev;
	String str;
} Token;

Token *tokenize_source(String *source);

#endif