#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include <stdio.h>

static void parser_skip_single_char(Token **start_tok, char c) {
	Token *token = *start_tok;
	if (token->type != TOKEN_SINGLE || token->str.data[0] != c) fatal_invalid_syntax();
	*start_tok = token->next;
}

static AstNode *parse_primary_expression(Token **start_tok) {
	
}

static AstNode *parse_unary_op_expression(Token **start_tok) {
	if (!TOKEN_IS_SINGLE_CHAR(*start_tok, '-') && !TOKEN_IS_SINGLE_CHAR(*start_tok, '!'))
		return parse_primary_expression(start_tok);
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
	parser_skip_single_char(start_tok, '(');
	AstNode *root = AstNode_new(AST_IF, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_single_char(start_tok, ')');
	parser_skip_single_char(start_tok, '{');
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_single_char(start_tok, '}');
	
	// parse optional else block
	if ((*start_tok)->type == TOKEN_KEYWORD && String_cmparr(&(*start_tok)->str, "else")) {
		*start_tok = (*start_tok)->next;
		parser_skip_single_char(start_tok, '{');
		DynArr_resize(&root->subs, 3);
		AstNode_addsub(root, parse_statement_list(start_tok, 1));
		parser_skip_single_char(start_tok, '}');
	}
	
	return root;
}

static AstNode *parse_while_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	parser_skip_single_char(start_tok, '(');
	AstNode *root = AstNode_new(AST_WHILE, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_single_char(start_tok, ')');
	parser_skip_single_char(start_tok, '{');
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_single_char(start_tok, '}');
	return root;
}

static AstNode *parse_async_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	AstNode *root = AstNode_new(AST_ASYNC, 1);
	parser_skip_single_char(start_tok, '{');
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_single_char(start_tok, '}');
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
		if (token->type != TOKEN_SINGLE) fatal_invalid_syntax();
		if (String_cmparr(&token->str, "=") == 0) {	// assignment_statement
			if (!ASTNODETYPE_IS_ASSIGNMENT_LVALUE(expr->type))
				fatal_invalid_syntax();
			parser_skip_single_char(start_tok, '=');
			AstNode *rvalue = parse_expression(start_tok);
			parser_skip_single_char(start_tok, ';');
			AstNode *root = AstNode_new(AST_ASSIGNMENT, 2);
			AstNode_addsub(root, expr);
			AstNode_addsub(root, rvalue);
			return root;
		} else {	// expression_statement
			parser_skip_single_char(start_tok, ';');
			return expr;
		}
	}
}

AstNode *parse_statement_list(Token **start_tok, int is_block) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	Token *token = *start_tok;
	while (token->type != TOKEN_EOF && !TOKEN_IS_SINGLE_CHAR(token, '}')) {
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
	// TODO: run interpreter (maybe generate bytecode?)
}
