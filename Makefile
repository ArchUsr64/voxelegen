all: main

export CC = clang
export CCFLAGS = -std=c99 -pedantic -Wall -g

LD = mold
LDFLAGS = -L lib/glfw/src -lglfw -lm

SOURCES = $(wildcard src/*.o)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
OBJECTS += lib/glad/gl.o

main: lib src $(OBJECTS)
	$(CC) -fuse-ld=$(LD) $(LDFLAGS) $(OBJECTS)

.PHONY: clean lib src

lib:
	$(MAKE) -C lib

src:
	$(MAKE) -C src

clean:
	-rm a.out
	-$(MAKE) -C lib clean
	-$(MAKE) -C src clean
