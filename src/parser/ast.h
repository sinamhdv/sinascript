#include "../types/string-view.h"
#include "../types/number.h"
#include "../types/dynarr.h"

#ifndef MYHEADER_AST_H
#define MYHEADER_AST_H

typedef enum AstNodeType {
	AST_STATEMENT_LIST = 0,	// program
	
	// statements
	AST_IF,
	AST_WHILE,
	AST_ASYNC,
	AST_ASSIGNMENT,

	// expressions
	AST_EXPR_LIST,	// comma-separated list of expressions
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
	union {
		String ident;	// identifier name
		String str;		// string literal value
		Number num;		// number literal value
		struct {
			char op[2];	// operator for binary/unary ops
			DynArr subs;	// generic node
		};
	};
} AstNode;

// size will be the initial size of DynArr at first or for
// nodes that can't have subs, it'll be the length of the string stored in them.
AstNode *AstNode_new(AstNodeType type, size_t size);

// returns -1 if the node cannot have subs, 0 on success
int AstNode_addsub(AstNode *node, AstNode *sub);

#define ASTNODETYPE_CAN_HAVE_SUBS(t) ((t) != AST_NUMBER && (t) != AST_STRING && (t) != AST_IDENTIFIER)
#define ASTNODETYPE_IS_ASSIGNMENT_LVALUE(t) ((t) == AST_IDENTIFIER || (t) == AST_INDEX)

#endif
