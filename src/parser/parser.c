#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include <stdio.h>

static AstNode *parse_statement_list(Token *token) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	while (token->type != TOKEN_EOF && ) {
		AstNode_addsub(root, parse_statement(&token));
	}
}

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_statement_list(tokens);
	// TODO: run interpreter (maybe generate bytecode?)
}
