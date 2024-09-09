#include "ast.h"
#include "../utils/utils.h"

int AstNodeType_can_have_subs(AstNodeType type) {
	return (type != AST_NUMBER
		&& type != AST_STRING
		&& type != AST_IDENTIFIER);
}

AstNode *AstNode_new(AstNodeType type, size_t extra_size) {
	AstNode *node;
	if (AstNodeType_can_have_subs(type)) {
		node = checked_malloc(sizeof(AstNode));
		DynArr_init(&node->subs, 0);
	} else {
		node = checked_malloc(sizeof(AstNode) + extra_size);
	}
	node->type = type;
	return node;
}

int AstNode_addsub(AstNode *node, AstNode *sub) {
	if (!AstNodeType_can_have_subs(node->type)) return -1;
	DynArr_push(&node->subs, sub);
	return 0;
}
