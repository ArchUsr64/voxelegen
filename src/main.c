#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "util.h"

#define PI 3.14

#define FRAGMENT_SHADER "shaders/fragment.glsl"
#define VERTEX_SHADER		"shaders/vertex.glsl"
#define ATLAS_PPM				"res/atlas.ppm"

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

unsigned char wireframe_mode = 0;
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
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		wireframe_mode = !wireframe_mode;
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	cursor_x = x;
	cursor_y = y;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, width, height);
}

int main(void) {
	struct ImageRGB atlas_texture;
	char *vert_shader = NULL, *frag_shader = NULL, *atlas_file = NULL;
	GLuint vbo, ebo, vao, vsh, fsh, rotation_uniform, shader_program;
	GLint vert_in_pos, vert_in_color;
	char log_status[512];
	GLFWwindow* window;
	int ret = 0, version;

	ret = glfwInit();
	if (ret != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialize glfw\n");
		goto exit;
	}
	ret = 0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create window\n");
		ret = -1;
		goto exit;
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

	vert_shader = read_file(VERTEX_SHADER);
	if (!vert_shader) {
		fprintf(stderr, "Failed to load '%s'\n", VERTEX_SHADER);
		ret = -1;
		goto exit;
	}

	vsh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsh, 1, (const GLchar* const*)&vert_shader, NULL);
	glCompileShader(vsh);

	glGetShaderiv(vsh, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		glGetShaderInfoLog(vsh, sizeof(log_status), NULL, log_status);
		fprintf(stderr, "VSH compilation failed:\n%s\n", log_status);
		ret = -1;
		goto exit;
	}

	frag_shader = read_file("shaders/fragment.glsl");
	if (!frag_shader) {
		fprintf(stderr, "Failed to load '%s'\n", FRAGMENT_SHADER);
		ret = -1;
		goto exit;
	}

	fsh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsh, 1, (const GLchar* const*)&frag_shader, NULL);
	glCompileShader(fsh);

	glGetShaderiv(fsh, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		glGetShaderInfoLog(fsh, sizeof(log_status), NULL, log_status);
		fprintf(stderr, "FSH compilation failed:\n%s\n", log_status);
		ret = -1;
		goto exit;
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
		ret = -1;
		goto exit;
	}

	rotation_uniform = glGetUniformLocation(shader_program, "rotation");
	vert_in_pos = glGetAttribLocation(shader_program, "in_pos");
	vert_in_color = glGetAttribLocation(shader_program, "in_color");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(
		vert_in_pos,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		NULL);
	glEnableVertexAttribArray(vert_in_pos);
	glVertexAttribPointer(
		vert_in_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vert_in_color);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices),
		indices,
		GL_STATIC_DRAW);

	atlas_file = read_file(ATLAS_PPM);
	if (!atlas_file) {
		fprintf(stderr, "Failed to read '%s'\n", ATLAS_PPM);
		ret = -1;
		goto exit;
	}
	atlas_texture = image_from_ppm(atlas_file);
	if (!atlas_texture.value) {
		fprintf(stderr, "Failed to parse ppm file '%s'\n", ATLAS_PPM);
		ret = -1;
		goto exit;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0., 0., 0., 0.);
		glClear(GL_COLOR_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);
		glUseProgram(shader_program);
		glUniform2f(
			rotation_uniform,
			2 * PI * cursor_x / screen_width,
			2 * PI * cursor_y / screen_height);
		glBindVertexArray(vao);
		glDrawElements(
			GL_TRIANGLES,
			sizeof(indices) / sizeof(unsigned int),
			GL_UNSIGNED_INT,
			0);

		glfwSwapBuffers(window);
	}

exit:
	free(vert_shader);
	free(frag_shader);
	free(atlas_file);
	glfwTerminate();

	return ret;
}
