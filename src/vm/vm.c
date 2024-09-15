#include "vm.h"
#include "../utils/utils.h"
#include "../utils/errors.h"
#include "../objects/ss-value.h"
#include "../objects/ss-string.h"
#include "../objects/ss-array.h"
#include <assert.h>
#include <string.h>

static SSValue vm_run_bin_op(AstNode *node) {
	// TODO
}

static SSValue vm_run_unary_op(AstNode *node) {
	// TODO
}

static SSValue vm_read_index(AstNode *node) {
	// TODO
}

static SSValue vm_get_identifier_value(AstNode *node) {
	// TODO
}

static SSArray *vm_eval_expression_list(AstNode *node) {
	DBGCHECK(node->type == AST_EXPR_LIST);
	SSArray *arr = SSArray_new(node->subs.size);

}

static SSValue vm_function_call(AstNode *node) {
	// TODO
}

static SSValue vm_make_number_literal(AstNode *node) {
	DBGCHECK(node->type == AST_NUMBER);
	SSValue value = {.type = SSVALUE_NUM, .value = (void*)(node->num)};
	return value;
}

static SSValue vm_make_str_literal(AstNode *node) {
	DBGCHECK(node->type == AST_STRING);
	SSValue value = {.type = SSVALUE_STR};
	SSString *str = SSString_new(node->str.size);
	str->size = node->str.size;
	memcpy(str->data, node->str.data, node->str.size);
	value.value = str;
	return value;
}

static SSValue vm_make_arr_literal(AstNode *node) {
	DBGCHECK(node->type == AST_ARRAY);
	DBGCHECK(node->subs.size == 1);
	SSValue value = {.type = SSVALUE_ARR};
	SSArray *arr = vm_eval_expression_list(node->subs.arr[0]);
	value.value = arr;
	return value;
}

SSValue vm_evaluate_expression(AstNode *node) {
	switch (node->type) {
		case AST_BIN_OP: return vm_run_bin_op(node);
		case AST_UNARY_OP: return vm_run_unary_op(node);
		case AST_INDEX: return vm_read_index(node);
		case AST_IDENTIFIER: return vm_get_identifier_value(node);
		case AST_FUNCTION_CALL: return vm_function_call(node);
		case AST_NUMBER: return vm_make_number_literal(node);
		case AST_STRING: return vm_make_str_literal(node);
		case AST_ARRAY: return vm_make_arr_literal(node);
		default: fatal_runtime_error(node);
	}
}

static SSValue *vm_get_lvalue(AstNode *node) {
	// TODO
}

static void vm_assign_variable(SSValue *var, SSValue value) {
	// TODO: maybe move this to variables.c?
}

static void vm_run_if_statement(AstNode *node) {
	DBGCHECK(node->type == AST_IF);
	DBGCHECK(node->subs.size == 2 || node->subs.size == 3);
	SSValue condition_val = vm_evaluate_expression(node->subs.arr[0]);
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
	while ((condition_val = vm_evaluate_expression(node->subs.arr[0])), SSVALUE_TRUTH_VAL(condition_val)) {
		vm_run_statement_list(node->subs.arr[1]);
	}
	// TODO: probably have to free the condition_val object here? (and also in if/assign/expr statements)
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
	SSValue *lvalue = vm_get_lvalue(node->subs.arr[0]);
	SSValue rvalue = vm_evaluate_expression(node->subs.arr[1]);
	vm_assign_variable(lvalue, rvalue);
}

static void vm_run_expression_statement(AstNode *node) {
	vm_evaluate_expression(node);
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
