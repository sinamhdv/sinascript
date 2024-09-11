#include "../types/string-view.h"
#include "token.h"

#ifndef MYHEADER_LEXER_H
#define MYHEADER_LEXER_H

Token *tokenize_source(String *source);

#endif