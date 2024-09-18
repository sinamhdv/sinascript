#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "../utils/errors.h"
#include "../vm/vm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

static size_t _get_string_literal_real_length(String *lit, Token *token) {
	size_t result = 0, i = 1, n = lit->size;
	while (lit->data[i] != '"') {
		if (lit->data[i] == '\\') {
			if (i + 2 == n) fatal_invalid_syntax(token);
			char c = lit->data[i + 1];
			if (c == 'x') {
				if (i + 5 > n) fatal_invalid_syntax(token);
				i += 4;
				result++;
			} else {
				i += 2;
				result++;
			}
		} else {
			result++;
			i++;
		}
	}
	return result;
}

#define IS_HEX_DIGIT(c) (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))

#define HEX_DIGIT_VALUE(c) (('0' <= (c) && (c) <= '9') ? (c) - '0' \
	: (('a' <= (c) && (c) <= 'f') ? (c) - 'a' : (c) - 'A'))

static char _parse_string_hex_escape(char *ptr, Token *token) {
	if (!IS_HEX_DIGIT(ptr[0]) || !IS_HEX_DIGIT(ptr[1]))
		fatal_invalid_syntax(token);
	return (HEX_DIGIT_VALUE(ptr[0]) << 4) | (HEX_DIGIT_VALUE(ptr[1]));
}

static void _do_parse_string_literal(String *lit, String *result, Token *token) {
	size_t i = 1;
	char *resptr = result->data;
	while (lit->data[i] != '"') {
		if (lit->data[i] == '\\') {
			char c = lit->data[i + 1];
			switch (c) {
				case '0': *resptr++ = '\0'; break;
				case 'a': *resptr++ = '\a'; break;
				case 'b': *resptr++ = '\b'; break;
				case 't': *resptr++ = '\t'; break;
				case 'n': *resptr++ = '\n'; break;
				case 'v': *resptr++ = '\v'; break;
				case 'f': *resptr++ = '\f'; break;
				case 'r': *resptr++ = '\r'; break;
				case '"': *resptr++ = '"'; break;
				case '\'': *resptr++ = '\''; break;
				case '\\': *resptr++ = '\\'; break;
				case 'x':
					*resptr++ = _parse_string_hex_escape(lit->data + i + 2, token);
					i += 2;
					break;
				default: fatal_invalid_syntax(token);
			}
			i += 2;
		} else {
			*resptr++ = lit->data[i++];
		}
	}
}

static AstNode *parse_string_literal(Token **start_tok) {
	size_t real_len = _get_string_literal_real_length(&(*start_tok)->str, *start_tok);
	AstNode *node = AstNode_new(AST_STRING, real_len);
	_do_parse_string_literal(&(*start_tok)->str, &node->str, *start_tok);
	*start_tok = (*start_tok)->next;
	return node;
}

static void parser_skip_operator(Token **start_tok, char *op) {
	if (!TOKEN_IS_OPERATOR(*start_tok, op))
		fatal_invalid_syntax(*start_tok);
	*start_tok = (*start_tok)->next;
}

static AstNode *parse_identifier(Token **start_tok) {
	AstNode *node = AstNode_new(AST_IDENTIFIER, (*start_tok)->str.size);
	memcpy(node->ident.data, (*start_tok)->str.data, node->ident.size);
	*start_tok = (*start_tok)->next;
	return node;
}

static AstNode *parse_number_literal(Token **start_tok) {
	char digits_buf[32];
	size_t smaller_size = (*start_tok)->str.size;
	if (sizeof(digits_buf) - 1 < smaller_size)
		smaller_size = sizeof(digits_buf) - 1;
	memcpy(digits_buf, (*start_tok)->str.data, smaller_size);
	digits_buf[smaller_size] = 0;

	Number num = (Number)strtoll(digits_buf, NULL, 10);
	if (num == (Number)LLONG_MAX && errno == ERANGE)
		fatal_invalid_syntax(*start_tok);
	AstNode *node = AstNode_new(AST_NUMBER, 0);
	node->num = num;
	*start_tok = (*start_tok)->next;
	return node;
}

static AstNode *parse_bracket_expression(Token **start_tok) {
	parser_skip_operator(start_tok, "(");
	AstNode *root = parse_expression(start_tok);
	parser_skip_operator(start_tok, ")");
	return root;
}

static AstNode *parse_expression_list(Token **start_tok, char *ending_op) {
	AstNode *root = AstNode_new(AST_EXPR_LIST, 0);
	while ((*start_tok)->type != TOKEN_EOF && !TOKEN_IS_OPERATOR(*start_tok, ending_op)) {
		AstNode_addsub(root, parse_expression(start_tok));
		if (!TOKEN_IS_OPERATOR(*start_tok, ",")) break;
		parser_skip_operator(start_tok, ",");
	}
	if (!TOKEN_IS_OPERATOR(*start_tok, ending_op))
		fatal_invalid_syntax(*start_tok);
	return root;
}

static AstNode *parse_array_literal(Token **start_tok) {
	parser_skip_operator(start_tok, "[");
	AstNode *root = AstNode_new(AST_ARRAY, 1);
	AstNode_addsub(root, parse_expression_list(start_tok, "]"));
	parser_skip_operator(start_tok, "]");
	return root;
}

static AstNode *parse_literal(Token **start_tok) {
	if (TOKEN_IS_OPERATOR(*start_tok, "["))
		return parse_array_literal(start_tok);
	switch ((*start_tok)->type) {
		case TOKEN_STRING:
			return parse_string_literal(start_tok);
		case TOKEN_NUMBER:
			return parse_number_literal(start_tok);
		default:
			fatal_invalid_syntax(*start_tok);
	}
}

static AstNode *parse_primary_expression(Token **start_tok) {
	if ((*start_tok)->type == TOKEN_IDENTIFIER)
		return parse_identifier(start_tok);
	else if (TOKEN_IS_OPERATOR(*start_tok, "("))
		return parse_bracket_expression(start_tok);
	return parse_literal(start_tok);
}

static AstNode *parse_function_call_expression(Token **start_tok) {
	AstNode *func = parse_primary_expression(start_tok);
	if (!TOKEN_IS_OPERATOR(*start_tok, "("))
		return func;
	if (func->type != AST_IDENTIFIER)
		fatal_invalid_syntax(*start_tok);
	parser_skip_operator(start_tok, "(");
	AstNode *arg_list = parse_expression_list(start_tok, ")");
	parser_skip_operator(start_tok, ")");
	AstNode *root = AstNode_new(AST_FUNCTION_CALL, 2);
	AstNode_addsub(root, func);
	AstNode_addsub(root, arg_list);
	return root;
}

static AstNode *parse_index_expression(Token **start_tok) {
	AstNode *array = parse_function_call_expression(start_tok);
	if (!TOKEN_IS_OPERATOR(*start_tok, "["))
		return array;
	parser_skip_operator(start_tok, "[");
	AstNode *index_val = parse_expression(start_tok);
	parser_skip_operator(start_tok, "]");
	AstNode *root = AstNode_new(AST_INDEX, 2);
	AstNode_addsub(root, array);
	AstNode_addsub(root, index_val);
	return root;
}

static AstNode *parse_unary_op_expression(Token **start_tok) {
	if (!TOKEN_IS_OPERATOR(*start_tok, "-") && !TOKEN_IS_OPERATOR(*start_tok, "!"))
		return parse_index_expression(start_tok);
	AstNode *root = AstNode_new(AST_UNARY_OP, 1);
	root->op[0] = (*start_tok)->str.data[0];
	*start_tok = (*start_tok)->next;
	AstNode_addsub(root, parse_unary_op_expression(start_tok));
	return root;
}

// loop implementation so we get left-to-right order for sth like (1 - 2 - 3)
static AstNode *parse_bin_op_expression(Token **start_tok, int level) {
	if (IS_PAST_LAST_BIN_OP_LEVEL(level))
		return parse_unary_op_expression(start_tok);
	AstNode *l_expr = parse_bin_op_expression(start_tok, level + 1);
	AstNode *cur_root = l_expr;
	while (TOKEN_IS_BIN_OP_LEVEL(*start_tok, level)) {
		char op[2] = {(*start_tok)->str.data[0],
			(*start_tok)->str.size > 1 ? (*start_tok)->str.data[1] : 0};
		*start_tok = (*start_tok)->next;
		AstNode *r_expr = parse_bin_op_expression(start_tok, level + 1);
		AstNode *new_root = AstNode_new(AST_BIN_OP, 2);
		new_root->op[0] = op[0];
		new_root->op[1] = op[1];
		AstNode_addsub(new_root, cur_root);
		AstNode_addsub(new_root, r_expr);
		cur_root = new_root;
	}
	return cur_root;
}

AstNode *parse_expression(Token **start_tok) {
	return parse_bin_op_expression(start_tok, 0);
}

static AstNode *parse_if_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	parser_skip_operator(start_tok, "(");
	AstNode *root = AstNode_new(AST_IF, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_operator(start_tok, ")");
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	
	// parse optional else block
	if ((*start_tok)->type == TOKEN_KEYWORD && String_cmparr(&(*start_tok)->str, "else") == 0) {
		*start_tok = (*start_tok)->next;
		parser_skip_operator(start_tok, "{");
		DynArr_resize(&root->subs, 3);
		AstNode_addsub(root, parse_statement_list(start_tok, 1));
		parser_skip_operator(start_tok, "}");
	}
	
	return root;
}

static AstNode *parse_while_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	parser_skip_operator(start_tok, "(");
	AstNode *root = AstNode_new(AST_WHILE, 2);
	AstNode_addsub(root, parse_expression(start_tok));
	parser_skip_operator(start_tok, ")");
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	return root;
}

static AstNode *parse_async_statement(Token **start_tok) {
	*start_tok = (*start_tok)->next;
	AstNode *root = AstNode_new(AST_ASYNC, 1);
	parser_skip_operator(start_tok, "{");
	AstNode_addsub(root, parse_statement_list(start_tok, 1));
	parser_skip_operator(start_tok, "}");
	return root;
}

static AstNode *parse_statement(Token **start_tok) {
	Token *token = *start_tok;
	if (token->type == TOKEN_KEYWORD) {
		if (String_cmparr(&token->str, "if") == 0) {
			return parse_if_statement(start_tok);
		} else if (String_cmparr(&token->str, "while") == 0) {
			return parse_while_statement(start_tok);
		} else if (String_cmparr(&token->str, "async") == 0) {
			return parse_async_statement(start_tok);
		} else {
			fatal_invalid_syntax(*start_tok);
		}
	} else {	// expression_statement | assignment_statement
		AstNode *expr = parse_expression(start_tok);
		if (TOKEN_IS_OPERATOR((*start_tok), "=")) {	// assignment_statement
			if (!ASTNODETYPE_IS_ASSIGNMENT_LVALUE(expr->type))
				fatal_invalid_syntax((*start_tok));
			parser_skip_operator(start_tok, "=");
			AstNode *rvalue = parse_expression(start_tok);
			parser_skip_operator(start_tok, ";");
			AstNode *root = AstNode_new(AST_ASSIGNMENT, 2);
			AstNode_addsub(root, expr);
			AstNode_addsub(root, rvalue);
			return root;
		} else {	// expression_statement or invalid syntax
			parser_skip_operator(start_tok, ";");
			return expr;
		}
	}
}

AstNode *parse_statement_list(Token **start_tok, int is_block) {
	AstNode *root = AstNode_new(AST_STATEMENT_LIST, 0);
	while ((*start_tok)->type != TOKEN_EOF && !TOKEN_IS_OPERATOR(*start_tok, "}")) {
		AstNode_addsub(root, parse_statement(start_tok));
	}
	if ((is_block && (*start_tok)->type == TOKEN_EOF)
		|| (!is_block && (*start_tok)->type != TOKEN_EOF))
		fatal_invalid_syntax(*start_tok);
	return root;
}

#ifdef DEBUG
static void debug_ast(AstNode *root, int level) {
	for (int i = 0; i < level; i++) putchar('\t');
	char *ast_type_names[] = {
	"AST_STATEMENT_LIST",
	"AST_IF",
	"AST_WHILE",
	"AST_ASYNC",
	"AST_ASSIGNMENT",
	"AST_EXPR_LIST",
	"AST_BIN_OP",
	"AST_UNARY_OP",
	"AST_INDEX",
	"AST_IDENTIFIER",
	"AST_FUNCTION_CALL",
	"AST_NUMBER",
	"AST_STRING",
	"AST_ARRAY",
	};
	if (root->type >= sizeof(ast_type_names)/sizeof(char*)) {
		printf("Invalid type!: %d\n", root->type);
		exit(1);
	}
	printf("%s(", ast_type_names[root->type]);
	switch (root->type) {
		case AST_NUMBER:
			printf("%ld)\n", root->num);
			break;
		case AST_STRING:
			fwrite(root->str.data, 1, root->str.size, stdout);
			printf(")\n");
			break;
		case AST_IDENTIFIER:
			fwrite(root->str.data, 1, root->str.size, stdout);
			printf(")\n");
			break;
		default:
			printf("%c%c) #%lu\n", root->op[0], root->op[1], root->subs.size);
			for (size_t i = 0; i < root->subs.size; i++) {
				debug_ast(root->subs.arr[i], level + 1);
			}
			break;
	}
}
#endif

void run_source(String *source) {
	Token *tokens = tokenize_source(source);
	AstNode *ast = parse_statement_list(&tokens, 0);
#ifdef DEBUG
	if (getenv("DEBUG_AST") != NULL) {
		debug_ast(ast, 0);
	}
#endif
	// TODO: free tokens (or not? useful for showing precise error locations to the user?)
	// TODO: generate bytecode?
	vm_main(ast);
	// TODO free ast and all other resources (except source) before returning
}
