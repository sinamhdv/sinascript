#include "errors.h"
#include <stdlib.h>
#include <stdio.h>

void fatal_invalid_syntax(Token *location) {
#ifdef DEBUG
	if (location != NULL) {
		fprintf(stderr, "Syntax Error: ");
		fputc('\'', stderr);
		fwrite(location->str.data, 1, location->str.size, stderr);
		fputc('\'', stderr);
		fputc('\n', stderr);
	} else {
		fprintf(stderr, "Syntax Error\n");
	}
#endif
	exit(1);
}

void fatal_runtime_error(AstNode *location) {
#ifdef DEBUG
	if (location != NULL) {
		fprintf(stderr, "Runtime Error: %d\n", location->type);
	} else {
		fprintf(stderr, "Runtime Error\n");
	}
#endif
	exit(1);
}
