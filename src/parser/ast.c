#include "ast.h"
#include "../utils/utils.h"

AstNode *AstNode_new(AstNodeType type, size_t extra_size) {
	AstNode *node;
	if (ASTNODETYPE_CAN_HAVE_SUBS(type)) {
		node = checked_malloc(sizeof(AstNode));
		DynArr_init(&node->subs, 0);
	} else {
		node = checked_malloc(sizeof(AstNode) + extra_size);
	}
	node->type = type;
	return node;
}

int AstNode_addsub(AstNode *node, AstNode *sub) {
	if (!ASTNODETYPE_CAN_HAVE_SUBS(node->type)) return -1;
	DynArr_push(&node->subs, sub);
	return 0;
}
