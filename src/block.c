#include <string.h>
#include <stdlib.h>

#include "block.h"

CREATE_VEC(vertex)

#define VERTICES_PER_FACE 4
#define INDICES_PER_FACE	6

// clang-format off
#define BASE_FACES_POS_X \
	{VERT_1, {0.5, 0.5}}, \
	{VERT_5, {1.0, 0.5}}, \
	{VERT_6, {1.0, 0.0}}, \
	{VERT_2, {0.5, 0.0}}  \

#define BASE_FACES_POS_X_INDICES {0, 1, 3, 1, 2, 3}

#define BASE_FACES_NEG_X \
	{VERT_0, {1.0, 0.5}}, \
	{VERT_3, {1.0, 0.0}}, \
	{VERT_7, {0.5, 0.0}}, \
	{VERT_4, {0.5, 0.5}}  \

#define BASE_FACES_NEG_X_INDICES {0, 1, 2, 0, 2, 3}

#define BASE_FACES_POS_Y \
	{VERT_2, {0.5, 0.5}}, \
	{VERT_3, {0.0, 0.5}}, \
	{VERT_7, {0.0, 0.0}}, \
	{VERT_6, {0.5, 0.0}}  \

#define BASE_FACES_POS_Y_INDICES {2, 1, 0, 3, 2, 0}

#define BASE_FACES_NEG_Y \
	{VERT_0, {1.0, 1.0}}, \
	{VERT_4, {0.5, 1.0}}, \
	{VERT_5, {0.5, 0.5}}, \
	{VERT_1, {1.0, 0.5}}  \

#define BASE_FACES_NEG_Y_INDICES {0, 2, 3, 0, 1, 2}

#define BASE_FACES_POS_Z \
	{VERT_4, {0.5, 1.0}}, \
	{VERT_5, {0.0, 1.0}}, \
	{VERT_6, {0.0, 0.5}}, \
	{VERT_7, {0.5, 0.5}}  \

#define BASE_FACES_POS_Z_INDICES {0, 2, 1, 0, 3, 2}

#define BASE_FACES_NEG_Z \
	{VERT_0, {0.0, 1.0}}, \
	{VERT_1, {0.5, 1.0}}, \
	{VERT_2, {0.5, 0.5}}, \
	{VERT_3, {0.0, 0.5}}  \

#define BASE_FACES_NEG_Z_INDICES {0, 1, 2, 0, 2, 3}

const vertex BASE_BLOCK[] = {
	BASE_FACES_NEG_Z,
	BASE_FACES_POS_X,
	BASE_FACES_POS_Z,
	BASE_FACES_NEG_X,
	BASE_FACES_POS_Y,
	BASE_FACES_NEG_Y,
};

const unsigned BASE_BLOCK_INDICES[] = {
	// -z
	0, 1, 2,
	0, 2, 3,
	// +x
	4, 5, 7,
	5, 6, 7,
	// +z
	8, 10, 9,
	8, 11, 10,
	// -x
	12, 13, 14,
	12, 14, 15,
	// +y
	18, 17, 16,
	19, 18, 16,
	// -y
	20, 22, 23,
	20, 21, 22,
};
// clang-format off

void create_block_data(
	int x,
	int y,
	int z,
	unsigned block_index,
	struct vec_vertex* vertices,
	struct vec_unsigned* indices) {
	unsigned block_index_offset = VERTICES_PER_FACE * INDICES_PER_FACE * block_index, i;
	for (i = 0; i < sizeof(BASE_BLOCK) / sizeof(vertex); i++) {
		vertex new_vert = BASE_BLOCK[i];
		new_vert.position[0] += x;
		new_vert.position[1] += y;
		new_vert.position[2] += z;
		vec_vertex_push(vertices, new_vert);
	}
	for (i = 0; i < sizeof(BASE_BLOCK_INDICES) / sizeof(unsigned); i++) {
		vec_unsigned_push(indices, BASE_BLOCK_INDICES[i] + block_index_offset);
	}
}

static void push_face_vertex_index_data(struct vec_vertex *vertices, struct vec_unsigned *indices, vertex* face_data, unsigned* index_data, int x, int y, int z, unsigned index_offset) {
	unsigned i;

	for (i = 0; i < VERTICES_PER_FACE; i++) {
		face_data[i].position[0] += x;
		face_data[i].position[1] += y;
		face_data[i].position[2] += z;
		vec_vertex_push(vertices, face_data[i]);
	}

	for (i = 0; i < INDICES_PER_FACE; i++)
		vec_unsigned_push(indices, index_data[i] + index_offset);
}

void create_block_data_by_parts(int x, int y, int z, bool pos_x, bool neg_x, bool pos_y, bool neg_y, bool pos_z, bool neg_z, struct vec_vertex *vertices, struct vec_unsigned *indices) {

	if (pos_x) {
		vertex face_data[] = {BASE_FACES_POS_X};
		unsigned index_data[] = BASE_FACES_POS_X_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}

	if (neg_x) {
		vertex face_data[] = {BASE_FACES_NEG_X};
		unsigned index_data[] = BASE_FACES_NEG_X_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}


	if (pos_y) {
		vertex face_data[] = {BASE_FACES_POS_Y};
		unsigned index_data[] = BASE_FACES_POS_Y_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}

	if (neg_y) {
		vertex face_data[] = {BASE_FACES_NEG_Y};
		unsigned index_data[] = BASE_FACES_NEG_Y_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}


	if (pos_z) {
		vertex face_data[] = {BASE_FACES_POS_Z};
		unsigned index_data[] = BASE_FACES_POS_Z_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}

	if (neg_z) {
		vertex face_data[] = {BASE_FACES_NEG_Z};
		unsigned index_data[] = BASE_FACES_NEG_Z_INDICES;
		push_face_vertex_index_data(vertices, indices, face_data, index_data, x, y, z, vertices->len);
	}

}
