#ifndef CHUNK_H_
#define CHUNK_H_

#define CHUNK_SIZE 16

#include "block.h"

struct chunk_mesh {
	struct vec_vertex vertices;
	struct vec_unsigned indices;
	float world_position[3];
};

struct chunk_mesh generate_chunk_mesh(int x, int y, int z);

#endif
