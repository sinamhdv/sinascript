#include "../objects/ss-array.h"
#include "../types/string-view.h"

#ifndef MYHEADER_BUILTINS_H
#define MYHEADER_BUILTINS_H

SSValue builtin_function_call(String *func_name, SSArray *args);

#endif
