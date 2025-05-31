#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "matrix.h"

#define PI 3.14

#define FRAGMENT_SHADER "shaders/fragment.glsl"
#define VERTEX_SHADER		"shaders/vertex.glsl"
#define ATLAS_PPM				"res/atlas.ppm"

const GLuint WIDTH = 958, HEIGHT = 1998;

// clang-format off
float vertices[] = {
	// x, y, z, u, v
	//front
	0.5, -0.5, -0.5, 1.0, 1.0, // bottom-right
	0.5, 0.5, -0.5, 1.0, 0.0, // top-right
	-0.5, 0.5, -0.5, 0.0, 0.0, // top-left
	-0.5, -0.5, -0.5, 0.0, 1.0, // bottom-left
	// back
	0.5, -0.5, 0.5, 0.0, 1.0, // bottom-right
	0.5, 0.5, 0.5, 0.0, 0.0, // top-right
	-0.5, 0.5, 0.5, 1.0, 0.0, // top-left
	-0.5, -0.5, 0.5, 1.0, 1.0, // bottom-left
	// top-only
	0.5, 0.5, 0.5, 1.0, 1.0, // top-right
	-0.5, 0.5, 0.5, 0.0, 1.0, // top-left
	// bottom-only
	0.5, -0.5, 0.5, 1.0, 0.0, // bottom-right
	-0.5, -0.5, 0.5, 0.0, 0.0, // bottom-left
};
unsigned int indices[] = {
	//sides
	0, 1, 3,
	1, 2, 3,
	0, 5, 1,
	0, 4, 5,
	6, 5, 4,
	7, 6, 4,
	2, 6, 7,
	3, 2, 7,
	//top
	1, 8, 2,
	2, 8, 9,
	//bottom
	3, 10, 0,
	11, 10, 3,
};
// clang-format on

enum BLOCK_TYPE {
	GRASS,
	DIRT,
	STONE,
	WATER,
};

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
	Matrix4 projection_matrix;
	struct ImageRGB atlas_image;
	char *vert_shader = NULL, *frag_shader = NULL, *atlas_file = NULL;
	GLuint vbo, ebo, vao, vsh, fsh, rotation_uniform, shader_program,
		atlas_texture, block_type_uniform, projection_matrix_uniform;
	GLint vert_in_pos, vert_in_uv;
	char log_status[512];
	GLFWwindow* window;
	int ret = 0, version;

	ret = glfwInit();
	if (ret != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialize glfw\n");
		goto exit;
	}
	ret = 0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	block_type_uniform = glGetUniformLocation(shader_program, "block_type");
	projection_matrix_uniform =
		glGetUniformLocation(shader_program, "projection_matrix");
	vert_in_pos = glGetAttribLocation(shader_program, "in_pos");
	if (vert_in_pos < 0) {
		fprintf(stderr, "Failed to query in_pos location");
		ret = vert_in_pos;
		goto exit;
	}
	vert_in_uv = glGetAttribLocation(shader_program, "in_uv");
	if (vert_in_uv < 0) {
		fprintf(stderr, "Failed to query in_uv location");
		ret = vert_in_uv;
		goto exit;
	}

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
		5 * sizeof(float),
		NULL);
	glEnableVertexAttribArray(vert_in_pos);
	glVertexAttribPointer(
		vert_in_uv,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vert_in_uv);

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
	atlas_image = image_from_ppm(atlas_file);
	if (!atlas_image.data) {
		fprintf(stderr, "Failed to parse ppm file '%s'\n", ATLAS_PPM);
		ret = -1;
		goto exit;
	}

	glGenTextures(1, &atlas_texture);
	glBindTexture(GL_TEXTURE_2D, atlas_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		atlas_image.width,
		atlas_image.height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		atlas_image.data);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	float near = 0.01, far = 1000, fov = PI / 2;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0., 0., 0., 0.);
		glClear(GL_COLOR_BUFFER_BIT);

		perespective_projection_matrix(
			near,
			far,
			fov,
			screen_width / screen_height,
			&projection_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);
		glUseProgram(shader_program);
		glUniform1i(block_type_uniform, GRASS);
		glUniform2f(
			rotation_uniform,
			-2 * PI * cursor_x / screen_width,
			-2 * PI * cursor_y / screen_height);
		glUniformMatrix4fv(
			projection_matrix_uniform,
			1,
			GL_TRUE,
			projection_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_texture);
		glBindVertexArray(vao);
		glDrawElements(
			GL_TRIANGLES,
			sizeof(indices) / sizeof(unsigned int),
			GL_UNSIGNED_INT,
			0);

		glfwSwapBuffers(window);
	}

	ret = 0;
exit:
	free(vert_shader);
	free(frag_shader);
	free(atlas_file);
	glfwTerminate();

	return ret;
}
