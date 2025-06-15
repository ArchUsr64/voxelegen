#ifndef BLOCK_H_
#define BLOCK_H_

#include "util.h"

/*
	       7+-------+6
	       /.      /|
	      / .     / |
	     /  .    /  |
	    3+-------+2 |
	    |   ....|../
	    |  /4   | /5
	+y  | /     |/
	 ^  |_______|
	 |  0       1
	 |
	 +---> +x
*/

typedef struct __attribute__((packed)) {
	float position[3];
	float uv[2];
} vertex;

DECLARE_VEC(vertex)

#define VERT_0 {0, 0, 0}
#define VERT_1 {1, 0, 0}
#define VERT_2 {1, 1, 0}
#define VERT_3 {0, 1, 0}
#define VERT_4 {0, 0, 1}
#define VERT_5 {1, 0, 1}
#define VERT_6 {1, 1, 1}
#define VERT_7 {0, 1, 1}

/*
	Texture atlas:

	(0,0)          (1,0)
	  +-------------+
	  |      |      |
	  | Top  |x-face|
	  |      |      |
	  |------+------|
	  |      |      |
	  |z-face|Bottom|
	  |      |      |
	  +------+------+
	(0,1)          (1,1)
*/

void create_block_data(
	int x,
	int y,
	int z,
	unsigned block_index,
	struct vec_vertex* vertices,
	struct vec_unsigned* indices);

#endif
