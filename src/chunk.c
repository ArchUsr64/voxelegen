#include "chunk.h"
#include "block.h"
#include "terrain.h"
#include "util.h"

struct chunk_mesh generate_chunk_mesh(int chunk_x, int chunk_y, int chunk_z) {
	struct chunk_mesh res = {
		.world_position =
			{CHUNK_SIZE * chunk_x, CHUNK_SIZE * chunk_y, CHUNK_SIZE * chunk_z}};
	unsigned block_index = 0;
	unsigned i, j, k;

	res.vertices = vec_vertex_new();
	res.indices = vec_unsigned_new();

	for (i = 0; i < CHUNK_SIZE; i++) {
		int pos_x = CHUNK_SIZE * chunk_x + i;
		for (j = 0; j < CHUNK_SIZE; j++) {
			int pos_y = CHUNK_SIZE * chunk_y + j;
			for (k = 0; k < CHUNK_SIZE; k++) {
				int pos_z = CHUNK_SIZE * chunk_z + k;
				if (terrain_generator(pos_x, pos_y, pos_z)) {
					create_block_data(
						pos_x,
						pos_y,
						pos_z,
						block_index,
						&res.vertices,
						&res.indices);
					block_index++;
				}
			}
		}
	}

	return res;
}
