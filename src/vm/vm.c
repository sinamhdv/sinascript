#include "vm.h"
#include "variables.h"
#include "../utils/utils.h"
#include "../utils/errors.h"
#include "../objects/ss-value.h"
#include "../objects/ss-string.h"
#include "../objects/ss-array.h"
#include "../heap/heap.h"
#include "../builtins/builtins.h"
#include <assert.h>
#include <string.h>

static SSNumber _bin_op_only_number(char op[2], SSValue lval, SSValue rval, AstNode *node) {
	if (lval.type != SSVALUE_NUM || rval.type != SSVALUE_NUM)
		fatal_runtime_error(node);
	SSNumber lnum = (SSNumber)lval.value;
	SSNumber rnum = (SSNumber)rval.value;
	switch (op[0]) {
		case '<':
			return op[1] == '=' ? (lnum <= rnum) : (lnum < rnum);
		case '>':
			return op[1] == '=' ? (lnum >= rnum) : (lnum > rnum);
		case '-':
			return lnum - rnum;
		case '*':
			return lnum * rnum;
		case '/':
			return lnum / rnum;
	}
	fatal_runtime_error(node);
}

static SSValue vm_run_bin_op(AstNode *node) {
	DBGCHECK(node->type == AST_BIN_OP);
	DBGCHECK(node->subs.size == 2);
	AstNode *lnode = node->subs.arr[0];
	AstNode *rnode = node->subs.arr[1];
	SSValue lval = vm_evaluate_expression(lnode);
	SSValue rval = vm_evaluate_expression(rnode);
	SSValue result;
	switch (node->op[0]) {
		case '|':
			result = (SSValue){.type = SSVALUE_NUM,
				.value = (void*)(size_t)(SSVALUE_TRUTH_VAL(lval) || SSVALUE_TRUTH_VAL(rval))};
			break;
		case '&':
			result = (SSValue){.type = SSVALUE_NUM,
				.value = (void*)(size_t)(SSVALUE_TRUTH_VAL(lval) && SSVALUE_TRUTH_VAL(rval))};
			break;
		case '<':
		case '>':
		case '-':
		case '*':
		case '/':
			result = (SSValue){.type = SSVALUE_NUM,
				.value = (void*)_bin_op_only_number(node->op, lval, rval, node)};
			break;
		case '=':
			DBGCHECK(node->op[1] == '=');
			result.type = SSVALUE_NUM;
			result.value = (void *)(size_t)(SSValue_cmp(lval, rval) == 0);
			break;
		case '+':
			result = SSValue_add(lval, rval, node);
			break;
		default: fatal_runtime_error(node);
	}
	ss_value_free_if_noref(lval);
	ss_value_free_if_noref(rval);
	return result;
}

static SSValue vm_run_unary_op(AstNode *node) {
	DBGCHECK(node->type == AST_UNARY_OP);
	DBGCHECK(node->subs.size == 1);
	SSValue operand = vm_evaluate_expression(node->subs.arr[0]);
	switch (node->op[0]) {
		case '-':
			if (operand.type != SSVALUE_NUM) {
				fatal_runtime_error(node);
			}
			operand.value = (void*)(-(SSNumber)operand.value);
			break;
		case '!': {
			void *new_value = (void*)(size_t)(!SSVALUE_TRUTH_VAL(operand));
			ss_value_free_if_noref(operand);	// heap-managed values become a number with this op
			operand.value = new_value;
			operand.type = SSVALUE_NUM;
			break;
		}
		default: fatal_runtime_error(node);
	}
	return operand;
}

static SSValue *vm_get_index_reference(AstNode *node) {
	DBGCHECK(node->type == AST_INDEX);
	DBGCHECK(node->subs.size == 2);
	AstNode *arr_name = node->subs.arr[0];
	AstNode *index_expr = node->subs.arr[1];
	if (arr_name->type != AST_IDENTIFIER)
		fatal_runtime_error(node);	// not implementing multidimentional array indexing yet
	SSValue *arr_ref = vm_get_var_reference(&(arr_name->ident), 0);
	if (arr_ref == NULL)
		fatal_runtime_error(node);
	SSValue index_val = vm_evaluate_expression(index_expr);
	if (index_val.type != SSVALUE_NUM)
		fatal_runtime_error(node);
	SSArray *arr = ((SSArray *)arr_ref->value);
	size_t access_idx = (size_t)index_val.value;
	if (access_idx >= arr->size)
		fatal_runtime_error(node);	// OOB array access
	return &arr->data[access_idx];
}

static SSValue vm_read_index(AstNode *node) {
	DBGCHECK(node->type == AST_INDEX);
	return *vm_get_index_reference(node);
}

static SSValue vm_get_identifier_value(AstNode *node) {
	DBGCHECK(node->type == AST_IDENTIFIER);
	SSValue *var_ref = vm_get_var_reference(&(node->ident), 0);
	if (var_ref == NULL)
		fatal_runtime_error(node);
	return *var_ref;
}

static SSArray *vm_eval_expression_list(AstNode *node) {
	DBGCHECK(node->type == AST_EXPR_LIST);
	SSArray *arr = SSArray_new(node->subs.size);
	for (size_t i = 0; i < node->subs.size; i++) {
		arr->data[i] = vm_evaluate_expression(node->subs.arr[i]);
		ss_value_inc_refcount(arr->data[i]);
	}
	return arr;
}

static SSValue vm_function_call(AstNode *node) {
	DBGCHECK(node->type == AST_FUNCTION_CALL);
	DBGCHECK(node->subs.size == 2);
	DBGCHECK(((AstNode *)node->subs.arr[0])->type == AST_IDENTIFIER);
	DBGCHECK(((AstNode *)node->subs.arr[1])->type == AST_EXPR_LIST);
	String *func_name = &((AstNode *)node->subs.arr[0])->ident;
	SSArray *arr = vm_eval_expression_list(node->subs.arr[1]);
	SSValue return_val = builtin_function_call(func_name, arr, node);
	ss_value_free_if_noref((SSValue){.type = SSVALUE_ARR, .value = arr});
	return return_val;
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
	DBGCHECK(node->type == AST_IDENTIFIER || node->type == AST_INDEX);
	switch (node->type) {
		case AST_IDENTIFIER:
			return vm_get_var_reference(&(node->ident), 1);
			break;
		case AST_INDEX:
			return vm_get_index_reference(node);
			break;
		default:
			fatal_runtime_error(node);
	}
}

static void vm_run_if_statement(AstNode *node) {
	DBGCHECK(node->type == AST_IF);
	DBGCHECK(node->subs.size == 2 || node->subs.size == 3);
	SSValue condition_val = vm_evaluate_expression(node->subs.arr[0]);
	int truth_val = SSVALUE_TRUTH_VAL(condition_val);
	ss_value_free_if_noref(condition_val);
	if (truth_val) {
		vm_run_statement_list(node->subs.arr[1]);
	} else if (node->subs.size == 3) {
		vm_run_statement_list(node->subs.arr[2]);
	}
}

static void vm_run_while_statement(AstNode *node) {
	DBGCHECK(node->type == AST_WHILE);
	DBGCHECK(node->subs.size == 2);
	SSValue condition_val = vm_evaluate_expression(node->subs.arr[0]);
	int truth_val = SSVALUE_TRUTH_VAL(condition_val);
	ss_value_free_if_noref(condition_val);
	while (truth_val) {
		vm_run_statement_list(node->subs.arr[1]);
		condition_val = vm_evaluate_expression(node->subs.arr[0]);
		truth_val = SSVALUE_TRUTH_VAL(condition_val);
		ss_value_free_if_noref(condition_val);
	}
}

static void vm_run_async_statement(AstNode *node) {
	// TODO: implement this with concurrency
	DBGCHECK(node->type == AST_ASYNC);
	DBGCHECK(node->subs.size == 1);
	vm_run_statement_list(node->subs.arr[0]);
}

static void vm_write_variable(SSValue *var, SSValue new_value) {
	ss_value_dec_refcount(*var);
	*var = new_value;
	ss_value_inc_refcount(*var);
}

static void vm_run_assignment_statement(AstNode *node) {
	DBGCHECK(node->type == AST_ASSIGNMENT);
	DBGCHECK(node->subs.size == 2);
	DBGCHECK(ASTNODETYPE_IS_ASSIGNMENT_LVALUE(((AstNode *)node->subs.arr[0])->type));
	SSValue *lvalue = vm_get_lvalue(node->subs.arr[0]);
	SSValue rvalue = vm_evaluate_expression(node->subs.arr[1]);
	vm_write_variable(lvalue, rvalue);
}

static void vm_run_expression_statement(AstNode *node) {
	SSValue value = vm_evaluate_expression(node);
	ss_value_free_if_noref(value);
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
