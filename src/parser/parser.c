#include "parser.h"
#include "lexer.h"
#include <stdio.h>

static AstNode *parse_tokens(Token *tokens) {
	
}

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_tokens(tokens);
	// TODO: run interpreter (maybe generate bytecode?)
}
