#include <stdio.h>
#include <stdlib.h>
#include "utils/string.h"

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return 1;
	}
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		return 1;
	}
	if (fseek(fp, 0, SEEK_END) != 0) return 1;
	long file_size = ftell(fp);
	if (file_size == -1) return 1;
	if (fseek(fp, 0, SEEK_SET) != 0) return 1;

	String *source = String_new(file_size);
	if (source == NULL) return 1;
	if (fread(source->data, 1, file_size, fp) != file_size) return 1;
	fclose(fp);

	// run_source(source);

	String_free(source);
	return 0;
}