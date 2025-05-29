all: glfw glad

.PHONY: clean glfw glad

clean:
	-$(MAKE) -C glfw clean
	-rm glad/gl.o

glad:
	cd glad && $(CC) -c $(CCFLAGS) src/gl.c -I include

glfw:
	cd glfw && cmake -D BUILD_SHARED_LIBS=ON . && make -j `nproc`
