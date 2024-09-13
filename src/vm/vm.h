#include "../parser/ast.h"

#ifndef MYHEADER_VM_H
#define MYHEADER_VM_H

void vm_run_statement_list(AstNode *node);
void vm_main(AstNode *ast);

#endif
