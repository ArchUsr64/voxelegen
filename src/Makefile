all: *.o

.PHONY: clean

CFLAGS = -I ../lib/glad/include -I ../lib/glfw/include

%.o: %.c
	$(CC) $(CCFLAGS) $(CFLAGS) $< -c

clean:
	-rm -v *.o
