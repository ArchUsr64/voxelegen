#include <stdlib.h>

#include "terrain_gen.h"

bool terrain_generator(int x, int y, int z) {
	return rand() & 1;
}
