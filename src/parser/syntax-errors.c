#include "syntax-errors.h"
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
