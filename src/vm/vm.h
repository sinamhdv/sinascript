#include "../parser/ast.h"
#include "../objects/ss-value.h"
#include <pthread.h>

extern pthread_mutex_t async_mutex;

#ifndef MYHEADER_VM_H
#define MYHEADER_VM_H

SSValue vm_evaluate_expression(AstNode *node);
void vm_run_statement_list(AstNode *node);
void vm_main(AstNode *ast);

#endif
