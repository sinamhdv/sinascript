#include "../types/string-view.h"
#include "token.h"
#include "ast.h"

#ifndef MYHEADER_PARSER_H
#define MYHEADER_PARSER_H

void run_source(String *source);
AstNode *parse_expression(Token **start_tok);
AstNode *parse_statement_list(Token **start_tok, int is_block);

#endif