#include "../types/string-view.h"
#include "token.h"

#ifndef MYHEADER_LEXER_H
#define MYHEADER_LEXER_H

void fatal_invalid_syntax(void);
Token *tokenize_source(String *source);

#endif