#include "../types/string-view.h"
#include "token.h"
#include "ast.h"

#ifndef MYHEADER_PARSER_H
#define MYHEADER_PARSER_H

void run_source(String *source);
AstNode *parse_expression(Token **start_tok);
AstNode *parse_statement_list(Token **start_tok, int is_block);

#define IS_PAST_LAST_BIN_OP_LEVEL(x) ((x) > 4)

// defines operator precedence
#define TOKEN_IS_BIN_OP_LEVEL(t, l) (((l) == 0 && TOKEN_IS_OPERATOR(t, "|")) \
	|| (((l) == 1) && TOKEN_IS_OPERATOR(t, "&")) \
	|| (((l) == 2) && (TOKEN_IS_OPERATOR(t, ">") \
		|| TOKEN_IS_OPERATOR(t, "<") \
		|| TOKEN_IS_OPERATOR(t, ">=") \
		|| TOKEN_IS_OPERATOR(t, "<=") \
		|| TOKEN_IS_OPERATOR(t, "==") \
		|| TOKEN_IS_OPERATOR(t, "!="))) \
	|| (((l) == 3) && (TOKEN_IS_OPERATOR(t, "+") \
		|| TOKEN_IS_OPERATOR(t, "-"))) \
	|| (((l) == 4) && (TOKEN_IS_OPERATOR(t, "*") \
		|| TOKEN_IS_OPERATOR(t, "/"))))

#endif