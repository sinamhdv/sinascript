#include "vm.h"
#include "../utils/utils.h"
#include "../utils/errors.h"
#include <assert.h>

static void vm_run_if_statement(AstNode *node) {

}

static void vm_run_while_statement(AstNode *node) {

}

static void vm_run_async_statement(AstNode *node) {

}

static void vm_run_assignment_statement(AstNode *node) {
	
}

static void vm_run_expression_statement(AstNode *node) {

}

static void vm_run_statement(AstNode *node) {
	switch (node->type) {
		case AST_IF: vm_run_if_statement(node); break;
		case AST_WHILE: vm_run_while_statement(node); break;
		case AST_ASYNC: vm_run_async_statement(node); break;
		case AST_ASSIGNMENT: vm_run_assignment_statement(node); break;
		default: vm_run_expression_statement(node); break;
	}
}

static void vm_run_statement_list(AstNode *node) {
	DBGCHECK(node->type == AST_STATEMENT_LIST);
	for (size_t i = 0; i < node->subs.size; i++) {
		vm_run_statement(node->subs.arr[i]);
	}
}

void vm_main(AstNode *ast) {
	vm_run_statement_list(ast);
}
