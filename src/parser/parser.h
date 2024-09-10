#include "../types/string-view.h"
#include "token.h"
#include "ast.h"

#ifndef MYHEADER_PARSER_H
#define MYHEADER_PARSER_H

void run_source(String *source);
AstNode *parse_expression(Token **start_tok);
AstNode *parse_statement_list(Token **start_tok, int is_block);

#define IS_PAST_LAST_BIN_OP_LEVEL(x) ((x) > 4)

#define TOKEN_IS_BIN_OP_LEVEL(t, l) (((l) == 0 && String_cmparr(&(t)->str, "|") == 0) \
	|| (((l) == 1) && String_cmparr(&(t)->str, "&") == 0) \
	|| (((l) == 2) && (String_cmparr(&(t)->str, ">") == 0 \
		|| String_cmparr(&(t)->str, "<") == 0 \
		|| String_cmparr(&(t)->str, ">=") == 0 \
		|| String_cmparr(&(t)->str, "<=") == 0 \
		|| String_cmparr(&(t)->str, "==") == 0)) \
	|| (((l) == 3) && (String_cmparr(&(t)->str, "+") == 0 \
		|| String_cmparr(&(t)->str, "-") == 0)) \
	|| (((l) == 4) && (String_cmparr(&(t)->str, "*") == 0 \
		|| String_cmparr(&(t)->str, "/") == 0)))

#endif