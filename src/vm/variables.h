#include "../objects/ss-value.h"
#include "../types/string-view.h"

#ifndef MYHEADER_VARIABLES_H
#define MYHEADER_VARIABLES_H

SSValue *vm_get_var_reference(String *identifier, int create_if_not_found);

#endif
