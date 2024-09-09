#include <stdlib.h>
#include <stdio.h>

void *checked_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		perror("malloc");
		exit(1);
	}
	return ptr;
}