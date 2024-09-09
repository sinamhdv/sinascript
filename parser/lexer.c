#include "lexer.h"
#include "../utils/utils.h"
#include <ctype.h>
#include <string.h>

static void fatal_invalid_syntax(void) {
	exit(1);
}

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

static int get_keyword_size_at(String *source, size_t i) {
	char *keywords[] = {"if", "else", "while", "async"};
	size_t keywords_size = sizeof(keywords) / sizeof(char *);
	for (size_t i = 0; i < keywords_size; i++) {
		size_t size = strlen(keywords[i]);
		if (i + size <= source->size) {
			if (memcmp(source->data + i, keywords[i], size) == 0) {
				if (i + size >= source->size) return size;
				char after_keyword = source->data[i + size];
				if (isalnum(after_keyword))
					return 0;
				return size;
			}
		}
	}
	return 0;
}

static Token *new_token(size_t str_size) {
	Token *token = checked_malloc(sizeof(Token) + str_size);
	token->next = token->prev = NULL;
	token->type = -1;
	token->str.size = str_size;
	memset(token->str.data, 0, str_size);
}

static Token *get_single_char_token(String *source, size_t i) {
	Token *token = new_token(1);
	token->type = TOKEN_SINGLE;
	token->str.data[0] = source->data[i];
	return token;
}

static Token *get_comparison_token(String *source, size_t i) {
	size_t data_size = (i + 1 < source->size && source->data[i + 1] == '=') ? 2 : 1;
	Token *token = new_token(data_size);
	token->type = TOKEN_COMPARE;
	token->str.data[0] = source->data[i];
	if (data_size > 1)
		token->str.data[1] = source->data[i + 1];
	return token;
}

static Token *get_string_literal_token(String *source, size_t i) {

}

static Token *get_num_literal_token(String *source, size_t i) {
	size_t digits_cnt = 0;
	while (i + digits_cnt < source->size && isdigit(source->data[i + digits_cnt]))
		digits_cnt++;
	if (i + digits_cnt < source->size && isalpha(source->data[i + digits_cnt])) {
		fatal_invalid_syntax();
	}
	Token *token = new_token(digits_cnt);
	token->type = TOKEN_NUMBER;
	memcpy(token->str.data, source->data + i, digits_cnt);
	return token;
}

static Token *get_keyword_token(String *source, size_t i) {
	size_t size = get_keyword_size_at(source, i);
	Token *token = new_token(size);
	token->type = TOKEN_KEYWORD;
	memcpy(token->str.data, source->data + i, size);
	return token;
}

static Token *get_identifier_token(String *source, size_t i) {
	size_t ident_size = 0;
	while (i + ident_size < source->size && isalnum(source->data[i + ident_size]))
		ident_size++;
	if (ident_size == 0) fatal_invalid_syntax();
	Token *token = new_token(ident_size);
	token->type = TOKEN_IDENTIFIER;
	memcpy(token->str.data, source->data + i, ident_size);
	return token;
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
				} else if (get_keyword_size_at(source, i) > 0) {
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
