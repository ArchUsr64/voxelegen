#ifndef UTIL_H_
#define UTIL_H_

#define UTIL_MAX_FILE_SIZE 4096

#define PPM_MAGIC 0x5036 /* "P6" */

#define CLAMP(val, min, max) ((val) > (max) ? (max) : ((val) < (min) ? (min) : val))

struct ImageRGB {
	unsigned int width, height;
	unsigned char* data;
};

struct ImageRGB image_from_ppm(char* file_data);

char* read_file(const char* filename);

#endif
