#include "../types/string-view.h"

#ifndef MYHEADER_LEXER_H
#define MYHEADER_LEXER_H

typedef enum TokenType {
	TOKEN_SINGLE = 0,	// single char token
	TOKEN_COMPARE,		// > < = >= <= ==
	TOKEN_STRING,		// string literal
	TOKEN_NUMBER,		// number literal
	TOKEN_KEYWORD,		// keyword
	TOKEN_IDENTIFIER,	// identifier
} TokenType;

typedef struct Token {
	struct Token *next;
	struct Token *prev;
	TokenType type;
	String str;
} Token;

Token *tokenize_source(String *source);

#endif