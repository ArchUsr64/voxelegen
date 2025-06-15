#include "chunk.h"
#include "block.h"
#include "terrain.h"
#include "util.h"

struct chunk_mesh generate_chunk_mesh(int chunk_x, int chunk_y, int chunk_z) {
	struct chunk_mesh res = {
		.world_position =
			{CHUNK_SIZE * chunk_x, CHUNK_SIZE * chunk_y, CHUNK_SIZE * chunk_z}};
	bool chunk_filled[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned i, j, k;

	res.vertices = vec_vertex_new();
	res.indices = vec_unsigned_new();

	for (i = 0; i < CHUNK_SIZE; i++) {
		int pos_x = CHUNK_SIZE * chunk_x + i;
		for (j = 0; j < CHUNK_SIZE; j++) {
			int pos_y = CHUNK_SIZE * chunk_y + j;
			for (k = 0; k < CHUNK_SIZE; k++) {
				int pos_z = CHUNK_SIZE * chunk_z + k;
				chunk_filled[i][j][k] = terrain_generator(pos_x, pos_y, pos_z);
			}
		}
	}

	for (i = 0; i < CHUNK_SIZE; i++) {
		for (j = 0; j < CHUNK_SIZE; j++) {
			for (k = 0; k < CHUNK_SIZE; k++) {
				if (!chunk_filled[i][j][k])
					continue;
				create_block_data_by_parts(
					i,
					j,
					k,
					(i == CHUNK_SIZE - 1) || !(chunk_filled[i + 1][j][k]),
					(i == 0) || !(chunk_filled[i - 1][j][k]),
					(j == CHUNK_SIZE - 1) || !(chunk_filled[i][j + 1][k]),
					(j == 0) || !(chunk_filled[i][j - 1][k]),
					(k == CHUNK_SIZE - 1) || !(chunk_filled[i][j][k + 1]),
					(k == 0) || !(chunk_filled[i][j][k - 1]),
					&res.vertices,
					&res.indices);
			}
		}
	}

	return res;
}
