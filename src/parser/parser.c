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

static AstNode *parse_unary_expression(Token **start_tok) {
	
}

static AstNode *parse_mul_expression(Token **start_tok) {
	
}

static AstNode *parse_add_expression(Token **start_tok) {
	
}

static AstNode *parse_cmp_expression(Token **start_tok) {
	
}

static AstNode *parse_and_expression(Token **start_tok) {
	
}

// TODO: generalize this function to parse_binop_expression? maybe not
// since add_expr and mul_expr have 2 operators and could be different.
// cmp is also different because of 2-byte operators.
static AstNode *parse_or_expression(Token **start_tok) {
	AstNode *l_expr = parse_and_expression(start_tok);
	if (TOKEN_IS_SINGLE_CHAR(*start_tok, '|')) {
		AstNode *r_expr = parse_or_expression(start_tok);
		AstNode *root = AstNode_new(AST_BIN_OP, 2);
		root->op[0] = '|';
		AstNode_addsub(root, l_expr);
		AstNode_addsub(root, r_expr);
		return root;
	}
	return l_expr;
}

AstNode *parse_expression(Token **start_tok) {
	return parse_or_expression(start_tok);
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
