#include "../utils/string-view.h"

#ifndef MYHEADER_PARSER_H
#define MYHEADER_PARSER_H

typedef int64_t Number;

typedef enum AstNodeType {
	AST_STATEMENT_LIST = 0,	// program
	
	// statements
	AST_IF,
	AST_WHILE,
	AST_ASYNC,
	AST_ASSIGNMENT,

	// expressions
	AST_BIN_OP,	// binary ops: | & < > <= >= == + - * /
	AST_UNARY_OP,	// unary ops: ! -
	AST_INDEX,	// array indexing: a[i]
	AST_IDENTIFIER,
	AST_FUNCTION_CALL,
	AST_NUMBER,	// number literal
	AST_STRING,	// string literal
	AST_ARRAY,	// array literal
} AstNodeType;

typedef struct AstNode {
	AstNodeType type;
	char op[2];	// operator for binary/unary ops
	union {
		char ident[];	// identifier name
		String str;		// string literal value
		Number num;		// number literal value

		struct {	// generic node
			size_t subcnt;	// number of children
			struct AstNode *subs[];
		};
	};
} AstNode;

void run_source(String *source);

#endif