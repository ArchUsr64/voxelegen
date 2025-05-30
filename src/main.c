#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "util.h"

#define PI 3.14

const GLuint WIDTH = 958, HEIGHT = 1998;

// clang-format off
float vertices[] = {
	// coord
	0.0, 0.5, 0.0,
	// color
	1.0, 0.0, 0.0,
	// coord
	-0.5, -0.5, 0.0,
	// color
	0.0, 0.0, 1.0,
	// coord
	0.5, -0.5, 0.0,
	// color
	0.0, 1.0, 0.0,
};
unsigned int indices[] = {
	0, 1, 2,
};
// clang-format on

float cursor_x = 0.0;
float cursor_y = 0.0;
float screen_width = 0.0;
float screen_height = 0.0;

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
	cursor_x = x;
	cursor_y = y;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, width, height);
}

int main(void) {
	GLuint vbo, ebo, vao, vsh, fsh, rotation_uniform, shader_program;
	char *vert_shader, *frag_shader;
	char log_status[512];
	GLFWwindow* window;
	int ret, version;

	ret = glfwInit();
	if (ret != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialize glfw\n");
		return ret;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	version = gladLoadGL(glfwGetProcAddress);
	printf(
		"GL %d.%d\n",
		GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version));

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	vert_shader = read_file("shaders/vertex.glsl");
	if (!vert_shader) {
		fprintf(stderr, "Failed to load vertex shader\n");
		glfwTerminate();
		return -1;
	}

	vsh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsh, 1, (const GLchar* const*)&vert_shader, NULL);
	glCompileShader(vsh);

	glGetShaderiv(vsh, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		glGetShaderInfoLog(vsh, sizeof(log_status), NULL, log_status);
		fprintf(stderr, "VSH compilation failed:\n%s\n", log_status);
		free(vert_shader);
		glfwTerminate();
		return -1;
	}

	frag_shader = read_file("shaders/fragment.glsl");
	if (!frag_shader) {
		fprintf(stderr, "Failed to load fragment shader\n");
		free(vert_shader);
		glfwTerminate();
		return -1;
	}

	fsh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsh, 1, (const GLchar* const*)&frag_shader, NULL);
	glCompileShader(fsh);

	glGetShaderiv(fsh, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		glGetShaderInfoLog(fsh, sizeof(log_status), NULL, log_status);
		fprintf(stderr, "FSH compilation failed:\n%s\n", log_status);
		free(vert_shader);
		free(frag_shader);
		glfwTerminate();
		return -1;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vsh);
	glAttachShader(shader_program, fsh);
	glLinkProgram(shader_program);

	glDeleteShader(vsh);
	glDeleteShader(fsh);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &ret);
	if (!ret) {
		glGetShaderInfoLog(shader_program, sizeof(log_status), NULL, log_status);
		fprintf(stderr, "FSH compilation failed:\n%s\n", log_status);
		free(vert_shader);
		free(frag_shader);
		glfwTerminate();
		return -1;
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices),
		indices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0., 0., 0., 0.);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		rotation_uniform = glGetUniformLocation(shader_program, "rotation");
		glUniform2f(rotation_uniform, 2 * PI * cursor_x / screen_width, 2 * PI * cursor_y / screen_height);
		glBindVertexArray(vao);
		glDrawElements(
			GL_TRIANGLES,
			sizeof(indices) / sizeof(unsigned int),
			GL_UNSIGNED_INT,
			0);

		glfwSwapBuffers(window);
	}

	free(vert_shader);
	free(frag_shader);
	glfwTerminate();

	return 0;
}
