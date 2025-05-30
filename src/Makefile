SOURCES = $(wildcard *.c *.h)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: $(OBJECTS)

.PHONY: clean

CFLAGS = -I ../lib/glad/include -I ../lib/glfw/include

%.o: %.c
	$(CC) $(CCFLAGS) $(CFLAGS) $< -c

clean:
	-rm -v *.o
