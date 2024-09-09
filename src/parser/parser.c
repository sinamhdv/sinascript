#include "parser.h"
#include "lexer.h"

void run_source(String *source) {
	Token *tokens = tokenize_source(source);

// #ifdef DEBUG
#include <stdio.h>
	puts("[DEBUG] Lexer result:");
	if (tokens == NULL) {
		puts("NULL");
	} else {
		Token *cur_token = tokens;
		do {
			switch (cur_token->type) {
				case TOKEN_SINGLE: printf("[SINGLE] "); break;
				case TOKEN_COMPARE: printf("[COMPARE] "); break;
				case TOKEN_STRING: printf("[STRING] "); break;
				case TOKEN_NUMBER: printf("[NUMBER] "); break;
				case TOKEN_KEYWORD: printf("[KEYWORD] "); break;
				case TOKEN_IDENTIFIER: printf("[IDENTIFIER] "); break;
				default: printf("[TOKEN_#%d] ", cur_token->type); break;
			}
			putchar('\'');
			fwrite(cur_token->str.data, 1, cur_token->str.size, stdout);
			putchar('\'');
			putchar('\n');
			cur_token = cur_token->next;
		} while (cur_token != tokens);
	}
// #endif

	// TODO: generate AST
	// TODO: run interpreter (maybe generate bytecode?)
}
