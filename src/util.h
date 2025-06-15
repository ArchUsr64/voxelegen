#ifndef UTIL_H_
#define UTIL_H_

#define UTIL_MAX_FILE_SIZE 4096

#define PPM_MAGIC 0x5036 /* "P6" */

#define VEC_INIT_SIZE 16

#define CLAMP(val, min, max) \
	((val) > (max) ? (max) : ((val) < (min) ? (min) : val))

struct ImageRGB {
	unsigned int width, height;
	unsigned char* data;
};

struct ImageRGB image_from_ppm(char* file_data);

char* read_file(const char* filename);

#define DECLARE_VEC(type) \
	struct vec_##type { \
		type* data; \
		unsigned len; \
		unsigned capacity; \
	}; \
	struct vec_##type vec_##type##_new(void); \
	void vec_##type##_push(struct vec_##type* vec, type value); \
	type vec_##type##_pop(struct vec_##type* vec);

#define CREATE_VEC(type) \
	struct vec_##type vec_##type##_new(void) { \
		struct vec_##type res; \
		res.data = malloc(VEC_INIT_SIZE * sizeof(type)); \
		res.len = 0; \
		res.capacity = VEC_INIT_SIZE; \
		return res; \
	} \
	void vec_##type##_push(struct vec_##type* vec, type value) { \
		if (vec->len >= vec->capacity) { \
			vec->capacity *= 2; \
			type* new_data = malloc(vec->capacity * sizeof(type)); \
			memcpy(new_data, vec->data, vec->len * sizeof(type)); \
			free(vec->data); \
			vec->data = new_data; \
		} \
		vec->data[vec->len] = value; \
		vec->len++; \
	} \
	type vec_##type##_pop(struct vec_##type* vec) { \
		return vec->data[--(vec->len)]; \
	}

DECLARE_VEC(int)
DECLARE_VEC(float)
DECLARE_VEC(unsigned)

#endif
