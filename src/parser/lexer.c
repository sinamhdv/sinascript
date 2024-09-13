#include "lexer.h"
#include "../utils/utils.h"
#include "syntax-errors.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

static size_t skip_whitespace(String *source, size_t i) {
	while (i < source->size && isspace(source->data[i]))
		i++;
	return i;
}

static int get_keyword_size_at(String *source, size_t i) {
	char *keywords[] = {"if", "else", "while", "async"};
	size_t keywords_size = sizeof(keywords) / sizeof(char *);
	for (size_t k = 0; k < keywords_size; k++) {
		size_t size = strlen(keywords[k]);
		if (i + size <= source->size) {
			if (memcmp(source->data + i, keywords[k], size) == 0) {
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

static Token *get_operator_token(String *source, size_t i) {
	size_t op_size = (i + 1 < source->size && source->data[i + 1] == '=') ? 2 : 1;
	Token *token = Token_new(op_size);
	token->type = TOKEN_OPERATOR;
	token->str.data[0] = source->data[i];
	if (op_size > 1)
		token->str.data[1] = source->data[i + 1];
	return token;
}

static Token *get_string_literal_token(String *source, size_t i) {
	size_t start_i = i;
	i++;
	while (i < source->size) {
		if (source->data[i] == '\\') {
			if (i + 1 >= source->size) fatal_invalid_syntax(NULL);
			i += 2;
		} else if (source->data[i] == '"') {
			break;
		} else {
			i++;
		}
	}
	if (i >= source->size || source->data[i] != '"') fatal_invalid_syntax(NULL);
	i++;
	Token *token = Token_new(i - start_i);
	token->type = TOKEN_STRING;
	memcpy(token->str.data, source->data + start_i, i - start_i);
	return token;
}

static Token *get_num_literal_token(String *source, size_t i) {
	size_t digits_cnt = 0;
	while (i + digits_cnt < source->size && isdigit(source->data[i + digits_cnt]))
		digits_cnt++;
	if (i + digits_cnt < source->size && isalpha(source->data[i + digits_cnt])) {
		fatal_invalid_syntax(NULL);
	}
	Token *token = Token_new(digits_cnt);
	token->type = TOKEN_NUMBER;
	memcpy(token->str.data, source->data + i, digits_cnt);
	return token;
}

static Token *get_keyword_token(String *source, size_t i) {
	size_t size = get_keyword_size_at(source, i);
	Token *token = Token_new(size);
	token->type = TOKEN_KEYWORD;
	memcpy(token->str.data, source->data + i, size);
	return token;
}

static Token *get_identifier_token(String *source, size_t i) {
	size_t ident_size = 0;
	while (i + ident_size < source->size && isalnum(source->data[i + ident_size]))
		ident_size++;
	if (ident_size == 0) fatal_invalid_syntax(NULL);
	Token *token = Token_new(ident_size);
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
			case '<':
			case '>':
			case '=':
				token = add_token_to_list(&tok_list, get_operator_token(source, i));
				break;
			case '"':
				token = add_token_to_list(&tok_list, get_string_literal_token(source, i));
				break;
			default: {
				if (isdigit(c)) {
					token = add_token_to_list(&tok_list, get_num_literal_token(source, i));
				} else if (get_keyword_size_at(source, i) > 0) {
					token = add_token_to_list(&tok_list, get_keyword_token(source, i));
				} else {
					token = add_token_to_list(&tok_list, get_identifier_token(source, i));
				}
				break;
			}
		}

#ifdef DEBUG
		switch (token->type) {
			case TOKEN_OPERATOR: fprintf(stderr, "[OPR] "); break;
			case TOKEN_STRING: fprintf(stderr, "[STR] "); break;
			case TOKEN_NUMBER: fprintf(stderr, "[NUM] "); break;
			case TOKEN_KEYWORD: fprintf(stderr, "[KEY] "); break;
			case TOKEN_IDENTIFIER: fprintf(stderr, "[IDN] "); break;
			default: break;
		}
		fputc('\'', stderr);
		fwrite(token->str.data, 1, token->str.size, stderr);
		fputc('\'', stderr);
		fputs("  ", stderr);
#endif

		i += token->str.size;
		i = skip_whitespace(source, i);
	}

	Token *eof_token = Token_new(0);
	eof_token->type = TOKEN_EOF;
	add_token_to_list(&tok_list, eof_token);

#ifdef DEBUG
	fprintf(stderr, "EOF\n\n");
#endif

	return tok_list;
}
