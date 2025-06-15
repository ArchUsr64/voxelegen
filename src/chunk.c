#include "chunk.h"
#include "block.h"
#include "terrain.h"
#include "util.h"

struct chunk_mesh generate_chunk_mesh(int chunk_x, int chunk_y, int chunk_z) {
	struct chunk_mesh res = {
		.world_position =
			{CHUNK_SIZE * chunk_x, CHUNK_SIZE * chunk_y, CHUNK_SIZE * chunk_z}};
	unsigned block_index = 0;
	unsigned x, y, z;

	res.vertices = vec_vertex_new();
	res.indices = vec_unsigned_new();

	for (x = 0; x < CHUNK_SIZE; x++) {
		for (y = 0; y < CHUNK_SIZE; y++) {
			for (z = 0; z < CHUNK_SIZE; z++) {
				if (terrain_generator(x, y, z)) {
					create_block_data(x, y, z, block_index, &res.vertices, &res.indices);
					block_index++;
				}
			}
		}
	}

	return res;
}
