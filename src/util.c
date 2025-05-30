#include <stdio.h>
#include <stdlib.h>

#include "util.h"

struct ImageRGB image_from_ppm(char* file_data) {
	struct ImageRGB ret;
	unsigned int color_max = -1;
	unsigned short magic_word = (file_data[0] << 8) | file_data[1];
	ret.data = NULL;

	if (magic_word != PPM_MAGIC) {
		fprintf(stderr, "Invalid magic number for ppm\n");
		return ret;
	}

	// Skip over the magic number and the following '\n'
	file_data += 3;

	while (*file_data == '#')
		while (*file_data != '\n')
			file_data++;

	// Skip over the '\n'
	file_data++;

	if (sscanf(file_data, "%ud", &ret.width) != 1) {
		fprintf(stderr, "Failed to read width from ppm\n");
		return ret;
	}
	if (sscanf(file_data, "%ud", &ret.height) != 1) {
		fprintf(stderr, "Failed to read height from ppm\n");
		return ret;
	}

	while (*file_data != '\n')
		file_data++;

	file_data++;

	sscanf(file_data, "%ud", &color_max);
	if (color_max != 255) {
		fprintf(stderr, "Invalid color max value: %d, expected 255\n", color_max);
		return ret;
	}

	while (*file_data != '\n')
		file_data++;

	file_data++;

	ret.data = (unsigned char*)file_data;
	return ret;
}

char* read_file(const char* filename) {
	char *ret = malloc(UTIL_MAX_FILE_SIZE * sizeof(char)), *i;
	FILE* fp = fopen(filename, "r");
	if (!fp)
		return NULL;

	i = ret;
	*i = fgetc(fp);
	while (*i != EOF) {
		i++;
		*i = fgetc(fp);
	}

	fclose(fp);
	return ret;
}
