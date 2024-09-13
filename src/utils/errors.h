#include "../parser/token.h"
#include "../parser/ast.h"

#ifndef MYHEADER_ERRORS_H
#define MYHEADER_ERRORS_H

void fatal_invalid_syntax(Token *location) __attribute__((noreturn));
void fatal_runtime_error(AstNode *location) __attribute__((noreturn));

#endif
