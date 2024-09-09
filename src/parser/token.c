#include "token.h"
#include "../utils/utils.h"
#include <string.h>

Token *add_token_to_list(Token **tok_list, Token *token) {
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

Token *Token_new(size_t str_size) {
	Token *token = checked_malloc(sizeof(Token) + str_size);
	token->next = token->prev = NULL;
	token->type = -1;
	token->str.size = str_size;
	memset(token->str.data, 0, str_size);
	return token;
}
