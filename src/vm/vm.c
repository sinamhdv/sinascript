#include "vm.h"
#include "../utils/utils.h"
#include "../utils/errors.h"
#include "../objects/ss-value.h"
#include <assert.h>

SSValue vm_run_expression(AstNode *node) {

}

static void vm_run_if_statement(AstNode *node) {
	DBGCHECK(node->type == AST_IF);
	DBGCHECK(node->subs.size == 2 || node->subs.size == 3);
	SSValue condition_val = vm_run_expression(node->subs.arr[0]);
	if (SSVALUE_TRUTH_VAL(condition_val)) {
		vm_run_statement_list(node->subs.arr[1]);
	} else if (node->subs.size == 3) {
		vm_run_statement_list(node->subs.arr[2]);
	}
}

static void vm_run_while_statement(AstNode *node) {
	DBGCHECK(node->type == AST_WHILE);
	DBGCHECK(node->subs.size == 2);
	SSValue condition_val;
	while ((condition_val = vm_run_expression(node->subs.arr[0])), SSVALUE_TRUTH_VAL(condition_val)) {
		vm_run_statement_list(node->subs.arr[1]);
	}
}

static void vm_run_async_statement(AstNode *node) {
	// TODO: implement this with concurrency
	DBGCHECK(node->type == AST_ASYNC);
	DBGCHECK(node->subs.size == 1);
	vm_run_statement_list(node->subs.arr[0]);
}

static void vm_run_assignment_statement(AstNode *node) {
	DBGCHECK(node->type == AST_ASSIGNMENT);
	DBGCHECK(node->subs.size == 2);
	DBGCHECK(ASTNODETYPE_IS_ASSIGNMENT_LVALUE(((AstNode *)node->subs.arr[0])->type));
	// TODO: find lvalue?
	SSValue rvalue = vm_run_expression(node->subs.arr[1]);
	// TODO: assign rvalue to lvalue
}

static void vm_run_expression_statement(AstNode *node) {
	vm_run_expression(node);
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

void vm_run_statement_list(AstNode *node) {
	DBGCHECK(node->type == AST_STATEMENT_LIST);
	for (size_t i = 0; i < node->subs.size; i++) {
		vm_run_statement(node->subs.arr[i]);
	}
}

void vm_main(AstNode *ast) {
	vm_run_statement_list(ast);
}
