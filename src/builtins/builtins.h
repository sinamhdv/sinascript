#include "../objects/ss-array.h"
#include "../objects/ss-string.h"
#include "../types/string-view.h"

#ifndef MYHEADER_BUILTINS_H
#define MYHEADER_BUILTINS_H

SSValue builtin_function_call(String *func_name, SSArray *args, AstNode *error_location_node);

#endif
