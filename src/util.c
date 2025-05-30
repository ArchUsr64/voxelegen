#include <stdio.h>
#include <stdlib.h>

#include "util.h"

char* read_file(const char* filename) {
	char* ret = malloc(UTIL_MAX_FILE_SIZE * sizeof(char)), *i;
	FILE* fp = fopen(filename, "r");
	if (!fp)
		return NULL;

	i = ret;
	*i = fgetc(fp);
	while(*i != EOF) {
		i++;
		*i = fgetc(fp);
	}

	fclose(fp);
	return ret;
}
