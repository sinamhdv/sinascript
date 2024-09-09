#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include <stdio.h>

static AstNode *parse_statement_list(Token *tokens) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	
}

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_statement_list(tokens);
	// TODO: run interpreter (maybe generate bytecode?)
}
