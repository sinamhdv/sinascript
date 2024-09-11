#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

static void parser_skip_operator(Token **start_tok, char *op) {
	if (!TOKEN_IS_OPERATOR(*start_tok, op)) fatal_invalid_syntax();
	*start_tok = (*start_tok)->next;
}

static AstNode *parse_identifier(Token **start_tok) {
	AstNode *node = AstNode_new(AST_IDENTIFIER, (*start_tok)->str.size);
	memcpy(node->ident.data, (*start_tok)->str.data, node->ident.size);
	*start_tok = (*start_tok)->next;
	return node;
}

static AstNode *parse_number_literal(Token **start_tok) {
	char digits_buf[32];
	size_t smaller_size = (*start_tok)->str.size;
	if (sizeof(digits_buf) - 1 < smaller_size)
		smaller_size = sizeof(digits_buf) - 1;
	memcpy(digits_buf, (*start_tok)->str.data, smaller_size);
	digits_buf[smaller_size] = 0;

	Number num = (Number)strtoll(digits_buf, NULL, 10);
	if (num == (Number)LLONG_MAX && errno == ERANGE)
		fatal_invalid_syntax();
	AstNode *node = AstNode_new(AST_NUMBER, 0);
	node->num = num;
	*start_tok = (*start_tok)->next;
	return node;
}

static AstNode *parse_string_literal(Token **start_tok) {
	
}

static AstNode *parse_bracket_expression(Token **start_tok) {
	parser_skip_operator(start_tok, "(");
	AstNode *root = parse_expression(start_tok);
	parser_skip_operator(start_tok, ")");
	return root;
}

static AstNode *parse_expression_list(Token **start_tok, char *ending_op) {
	AstNode *root = AstNode_new(AST_EXPR_LIST, 0);
	while ((*start_tok)->type != TOKEN_EOF && !TOKEN_IS_OPERATOR(*start_tok, ending_op)) {
		AstNode_addsub(root, parse_expression(start_tok));
		if (!TOKEN_IS_OPERATOR(*start_tok, ",")) break;
		parser_skip_operator(start_tok, ",");
	}
	if (!TOKEN_IS_OPERATOR(*start_tok, ending_op))
		fatal_invalid_syntax();
	return root;
}

static AstNode *parse_array_literal(Token **start_tok) {
	parser_skip_operator(start_tok, "[");
	AstNode *root = AstNode_new(AST_ARRAY, 1);
	AstNode_addsub(root, parse_expression_list(start_tok, "]"));
	parser_skip_operator(start_tok, "]");
	return root;
}

static AstNode *parse_literal(Token **start_tok) {
	if (TOKEN_IS_OPERATOR(*start_tok, "["))
		return parse_array_literal(start_tok);
	switch ((*start_tok)->type) {
		case TOKEN_STRING:
			return parse_string_literal(start_tok);
		case TOKEN_NUMBER:
			return parse_number_literal(start_tok);
		default:
			fatal_invalid_syntax();
	}
}

/*
static AstNode *parse_primary_expression(Token **start_tok) {
	switch ((*start_tok)->type) {
		case TOKEN_IDENTIFIER: {	// identifier | function_call | index_expr
			if (TOKEN_IS_OPERATOR((*start_tok)->next, "(")) {
				return parse_function_call_expression(start_tok);
			} else if (TOKEN_IS_OPERATOR((*start_tok)->next, "[")) {
				return parse_index_expression(start_tok);
			} else {
				return parse_identifier(start_tok);
			}
			break;
		}
		case TOKEN_STRING:
			return parse_string_literal(start_tok);
		case TOKEN_NUMBER:
			return parse_number_literal(start_tok);
		case TOKEN_OPERATOR:	// array_literal or '(' expr ')'
			if (TOKEN_IS_OPERATOR(*start_tok, "[")) {
				return parse_array_literal(start_tok);
			} else {
				return parse_bracket_expression(start_tok);
			}
		default:
			fatal_invalid_syntax();
	}
}
*/

static AstNode *parse_primary_expression(Token **start_tok) {
	if ((*start_tok)->type == TOKEN_IDENTIFIER)
		return parse_identifier(start_tok);
	else if (TOKEN_IS_OPERATOR(*start_tok, "("))
		return parse_bracket_expression(start_tok);
	return parse_literal(start_tok);
}

static AstNode *parse_function_call_expression(Token **start_tok) {
	AstNode *func = parse_primary_expression(start_tok);
	if (!TOKEN_IS_OPERATOR(*start_tok, "("))
		return func;
	parser_skip_operator(start_tok, "(");
	AstNode *arg_list = parse_expression_list(start_tok, ")");
	parser_skip_operator(start_tok, ")");
	AstNode *root = AstNode_new(AST_FUNCTION_CALL, 2);
	AstNode_addsub(root, func);
	AstNode_addsub(root, arg_list);
	return root;
}

static AstNode *parse_index_expression(Token **start_tok) {
	AstNode *array = parse_function_call_expression(start_tok);
	if (!TOKEN_IS_OPERATOR(*start_tok, "["))
		return array;
	parser_skip_operator(start_tok, "[");
	AstNode *index_val = parse_expression(start_tok);
	parser_skip_operator(start_tok, "]");
	AstNode *root = AstNode_new(AST_INDEX, 2);
	AstNode_addsub(root, array);
	AstNode_addsub(root, index_val);
	return root;
}

static AstNode *parse_unary_op_expression(Token **start_tok) {
	if (!TOKEN_IS_OPERATOR(*start_tok, "-") && !TOKEN_IS_OPERATOR(*start_tok, "!"))
		return parse_index_expression(start_tok);
	AstNode *root = AstNode_new(AST_UNARY_OP, 1);
	root->op[0] = (*start_tok)->str.data[0];
	*start_tok = (*start_tok)->next;
	AstNode_addsub(root, parse_unary_op_expression(start_tok));
	return root;
}

static AstNode *parse_bin_op_expression(Token **start_tok, int level) {
	if (IS_PAST_LAST_BIN_OP_LEVEL(level))
		return parse_unary_op_expression(start_tok);
	AstNode *l_expr = parse_bin_op_expression(start_tok, level + 1);
	if (TOKEN_IS_BIN_OP_LEVEL(*start_tok, level)) {
		char op[2] = {(*start_tok)->str.data[0],
			(*start_tok)->str.size > 1 ? (*start_tok)->str.data[1] : 0};
		AstNode *r_expr = parse_bin_op_expression(start_tok, level);
		AstNode *root = AstNode_new(AST_BIN_OP, 2);
		root->op[0] = op[0];
		root->op[1] = op[1];
		AstNode_addsub(root, l_expr);
		AstNode_addsub(root, r_expr);
		return root;
	}
	return l_expr;
}

AstNode *parse_expression(Token **start_tok) {
	return parse_bin_op_expression(start_tok, 0);
}

static AstNode *parse_if_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	parser_skip_operator(start_tok, "(");
	AstNode *root = AstNode_new(AST_IF, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_operator(start_tok, ")");
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	
	// parse optional else block
	if ((*start_tok)->type == TOKEN_KEYWORD && String_cmparr(&(*start_tok)->str, "else")) {
		*start_tok = (*start_tok)->next;
		parser_skip_operator(start_tok, "{");
		DynArr_resize(&root->subs, 3);
		AstNode_addsub(root, parse_statement_list(start_tok, 1));
		parser_skip_operator(start_tok, "}");
	}
	
	return root;
}

static AstNode *parse_while_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	parser_skip_operator(start_tok, "(");
	AstNode *root = AstNode_new(AST_WHILE, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_operator(start_tok, ")");
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	return root;
}

static AstNode *parse_async_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	AstNode *root = AstNode_new(AST_ASYNC, 1);
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	return root;
}

static AstNode *parse_statement(Token **start_tok) {
	Token *token = *start_tok;
	if (token->type == TOKEN_KEYWORD) {
		if (String_cmparr(&token->str, "if") == 0) {
			return parse_if_statement(start_tok);
		} else if (String_cmparr(&token->str, "while") == 0) {
			return parse_while_statement(start_tok);
		} else if (String_cmparr(&token->str, "async") == 0) {
			return parse_async_statement(start_tok);
		} else {
			fatal_invalid_syntax();
		}
	} else {	// expression_statement | assignment_statement
		AstNode *expr = parse_expression(start_tok);
		token = *start_tok;
		if (TOKEN_IS_OPERATOR(token, "=")) {	// assignment_statement
			if (!ASTNODETYPE_IS_ASSIGNMENT_LVALUE(expr->type))
				fatal_invalid_syntax();
			parser_skip_operator(start_tok, "=");
			AstNode *rvalue = parse_expression(start_tok);
			parser_skip_operator(start_tok, ";");
			AstNode *root = AstNode_new(AST_ASSIGNMENT, 2);
			AstNode_addsub(root, expr);
			AstNode_addsub(root, rvalue);
			return root;
		} else {	// expression_statement or invalid syntax
			parser_skip_operator(start_tok, ";");
			return expr;
		}
	}
}

AstNode *parse_statement_list(Token **start_tok, int is_block) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	Token *token = *start_tok;
	while (token->type != TOKEN_EOF && !TOKEN_IS_OPERATOR(token, "}")) {
		AstNode_addsub(root, parse_statement(&token));
	}
	if ((is_block && token->type == TOKEN_EOF)
		|| (!is_block && token->type != TOKEN_EOF))
		fatal_invalid_syntax();
	*start_tok = token;
	return root;
}

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_statement_list(&tokens, 0);
	// TODO: free tokens
	// TODO: run interpreter (maybe generate bytecode?)
	// TODO free ast and all other resources (except source) before returning
}
