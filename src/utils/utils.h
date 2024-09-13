#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef MYHEADER_UTILS_H
#define MYHEADER_UTILS_H

void *checked_malloc(size_t size);
void *checked_realloc(void *ptr, size_t size);

#ifdef DEBUG
#define DBGCHECK(x) assert(x)
#else
#define DBGCHECK(x) ;
#endif

#endif