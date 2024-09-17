#include "errors.h"
#include <stdlib.h>
#include <stdio.h>

void fatal_invalid_syntax(Token *location) {
#ifdef DEBUG
	fflush(stdout);
	if (location != NULL) {
		fprintf(stderr, "\nSyntax Error: ");
		fputc('\'', stderr);
		fwrite(location->str.data, 1, location->str.size, stderr);
		fputc('\'', stderr);
		fputc('\n', stderr);
	} else {
		fprintf(stderr, "\nSyntax Error\n");
	}
#endif
	exit(1);
}

void fatal_runtime_error(AstNode *location) {
#ifdef DEBUG
	fflush(stdout);
	if (location != NULL) {
		fprintf(stderr, "\nRuntime Error: %d\n", location->type);
	} else {
		fprintf(stderr, "\nRuntime Error\n");
	}
#endif
	exit(1);
}
