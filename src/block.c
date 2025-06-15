#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "block.h"

CREATE_VEC(vertex)

// clang-format off
const vertex BASE_BLOCK[] = {
	// 0123 front face
	{VERT_0, {0.0, 1.0}},
	{VERT_1, {0.5, 1.0}},
	{VERT_2, {0.5, 0.5}},
	{VERT_3, {0.0, 0.5}},
	// 1562 right face
	{VERT_1, {0.5, 0.5}},
	{VERT_5, {1.0, 0.5}},
	{VERT_6, {1.0, 0.0}},
	{VERT_2, {0.5, 0.0}},
	// 4567 back face
	{VERT_4, {0.5, 1.0}},
	{VERT_5, {0.0, 1.0}},
	{VERT_6, {0.0, 0.5}},
	{VERT_7, {0.5, 0.5}},
	// 0374 left face
	{VERT_0, {1.0, 0.5}},
	{VERT_3, {1.0, 0.0}},
	{VERT_7, {0.5, 0.0}},
	{VERT_4, {0.5, 0.5}},
	// 2376 top face
	{VERT_2, {0.5, 0.5}},
	{VERT_3, {0.0, 0.5}},
	{VERT_7, {0.0, 0.0}},
	{VERT_6, {0.5, 0.0}},
	// 0451 bottom face
	{VERT_0, {1.0, 1.0}},
	{VERT_4, {0.5, 1.0}},
	{VERT_5, {0.5, 0.5}},
	{VERT_1, {1.0, 0.5}},
};

const unsigned BASE_BLOCK_INDICES[] = {
	// // 0123 front face
	0, 1, 2,
	0, 2, 3,
	// 1562 right face
	4, 5, 7,
	5, 6, 7,
	// 4567 back face
	8, 10, 9,
	8, 11, 10,
	// 0374 left face
	12, 13, 14,
	12, 14, 15,
	// 2376 top face
	18, 17, 16,
	19, 18, 16,
	// 0451 bottom face
	20, 22, 23,
	20, 21, 22,
};
// clang-format off

void create_block_data(
	int pos_x,
	int pos_y,
	int pos_z,
	unsigned block_index,
	struct vec_vertex* vertices,
	struct vec_unsigned* indices) {
	unsigned block_index_offset = 24 * block_index, i;
	for (i = 0; i < sizeof(BASE_BLOCK) / sizeof(vertex); i++) {
		vertex new_vert = BASE_BLOCK[i];
		new_vert.position[0] += pos_x;
		new_vert.position[1] += pos_y;
		new_vert.position[2] += pos_z;
		vec_vertex_push(vertices, new_vert);
	}
	for (i = 0; i < sizeof(BASE_BLOCK_INDICES) / sizeof(unsigned); i++) {
		vec_unsigned_push(indices, BASE_BLOCK_INDICES[i] + block_index_offset);
	}
}
