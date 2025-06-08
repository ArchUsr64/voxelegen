#ifndef BLOCK_H_
#define BLOCK_H_

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

struct __attribute__((packed)) block_vert {
	float position[3];
	float uv[2];
};

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

// clang-format off
const struct block_vert BASE_BLOCK[] = {
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

#endif
