#include <stdlib.h>

#include "terrain.h"

bool terrain_generator(int x, int y, int z) {
	return rand() & 1;
}
