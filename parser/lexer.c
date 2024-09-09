#include "lexer.h"
#include <ctype.h>

static size_t skip_whitespace(String *source, size_t i) {
	while (i < source->size && isspace(source->data[i]))
		i++;
	return i;
}

static Token *add_token(Token **tok_list, Token *token) {
	if (*tok_list == NULL) {
		*tok_list = token;
		token->next = token->prev = token;
		return token;
	}
	Token *list_head = *tok_list;
	Token *list_end = list_head->prev;
	token->prev = list_end;
	token->next = list_head;
	list_head->prev = token;
	list_end->next = token;
	return token;
}

static int is_keyword(String *source, size_t i) {
	
}

static Token *get_single_char_token(String *source, size_t i) {

}

static Token *get_comparison_token(String *source, size_t i) {

}

static Token *get_string_literal_token(String *source, size_t i) {

}

static Token *get_num_literal_token(String *source, size_t i) {

}

static Token *get_keyword_token(String *source, size_t i) {

}

static Token *get_identifier_token(String *source, size_t i) {

}

Token *tokenize_source(String *source) {
	size_t i = 0;
	Token *tok_list = NULL;
	
	i = skip_whitespace(source, i);
	while (i < source->size) {
		char c = source->data[i];
		Token *token = NULL;
		switch (c) {
			case '[':
			case ']':
			case '(':
			case ')':
			case '{':
			case '}':
			case '+':
			case '-':
			case '*':
			case '/':
			case '&':
			case '|':
			case '!':
			case ';':
			case ',':
				token = add_token(&tok_list, get_single_char_token(source, i));
				break;
			case '<':
			case '>':
			case '=':
				token = add_token(&tok_list, get_comparison_token(source, i));
				break;
			case '"':
				token = add_token(&tok_list, get_string_literal_token(source, i));
				break;
			default: {
				if (isdigit(c)) {
					token = add_token(&tok_list, get_num_literal_token(source, i));
				} else if (is_keyword(source, i)) {
					token = add_token(&tok_list, get_keyword_token(source, i));
				} else {
					token = add_token(&tok_list, get_identifier_token(source, i));
				}
				break;
			}
		}
		i += token->str.size;
		i = skip_whitespace(source, i);
	}
	return tok_list;
}
