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

void *checked_realloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);
	if (ptr == NULL) {
		perror("realloc");
		exit(1);
	}
	return ptr;
}
