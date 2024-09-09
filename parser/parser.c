#include "parser.h"
#include "lexer.h"

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	// TODO: generate AST
	// TODO: run interpreter (maybe generate bytecode?)
}
