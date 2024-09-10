#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include <stdio.h>

static AstNode *parser_skip_single_char(Token **start_tok, char c) {

}

static AstNode *parse_expression(Token **start_tok) {

}

static AstNode *parse_if_statement(Token **start_tok) {
	// TODO
}

static AstNode *parse_while_statement(Token **start_tok) {
	// TODO
}

static AstNode *parse_async_statement(Token **start_tok) {
	// TODO
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
			AstNode *root = AstNode_new(AST_ASSIGNMENT, 0);
			DynArr_resize(&root->subs, 2);
			DynArr_push(&root->subs, expr);
			DynArr_push(&root->subs, rvalue);
			return root;
		} else {	// expression_statement
			parser_skip_single_char(start_tok, ';');
			return expr;
		}
	}
}

static AstNode *parse_statement_list(Token **start_tok, int is_block) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	Token *token = *start_tok;
	while (token->type != TOKEN_EOF && !TOKEN_IS_CLOSING_CURLY_BRACE(token)) {
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
