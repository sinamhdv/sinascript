#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include <stdio.h>

static AstNode *parse_statement_list(Token *tokens) {
	// AstNode *root = new_ast_node()
}

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_statement_list(tokens);
	// TODO: run interpreter (maybe generate bytecode?)
}
