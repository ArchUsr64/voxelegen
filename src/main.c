#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "matrix.h"
#include "camera.h"
#include "block.h"

#define MOUSE_SENS 1000

#define PI 3.14

#define FRAGMENT_SHADER "shaders/fragment.glsl"
#define VERTEX_SHADER		"shaders/vertex.glsl"
#define ATLAS_PPM				"res/atlas.ppm"

const GLuint WIDTH = 958, HEIGHT = 1998;

unsigned char wireframe_mode = 0;
float aspect_ratio = 1.0;
struct Camera camera = { 0 };
struct CameraKeys keys;

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		keys.in = 1;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		keys.in = 0;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		keys.left = 1;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		keys.left = 0;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		keys.out = 1;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		keys.out = 0;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		keys.right = 1;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		keys.right = 0;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		keys.up = 1;
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		keys.up = 0;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		keys.down = 1;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		keys.down = 0;
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		wireframe_mode = !wireframe_mode;
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	static float last_x = 0, last_y = 0;

	camera.yaw += (x - last_x) / MOUSE_SENS;
	camera.pitch += (y - last_y) / MOUSE_SENS;
	camera.pitch = CLAMP(camera.pitch, -PI / 2, PI / 2);

	last_x = x;
	last_y = y;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	aspect_ratio = (float)width / height;
	glViewport(0, 0, width, height);
}

int main(void) {
	char *vert_shader = NULL, *frag_shader = NULL, *atlas_file = NULL;
	GLuint vbo, ebo, vao, vsh, fsh, shader_program, atlas_texture,
		projection_matrix_uniform, view_matrix_uniform,
		tick_uniform;
	GLint vert_in_pos, vert_in_uv;
	struct ImageRGB atlas_image;
	Matrix4 projection_matrix, view_matrix;
	unsigned int tick = 0;
	int ret = 0, version;
	char log_status[512];
	GLFWwindow* window;

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

	projection_matrix_uniform =
		glGetUniformLocation(shader_program, "projection_matrix");
	view_matrix_uniform =
		glGetUniformLocation(shader_program, "view_matrix");
	tick_uniform = glGetUniformLocation(shader_program, "tick");
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(BASE_BLOCK), BASE_BLOCK, GL_STATIC_DRAW);
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
		sizeof(BASE_BLOCK_INDICES),
		BASE_BLOCK_INDICES,
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
	glEnable(GL_DEPTH_TEST);

	float near = 0.01, far = 1000, fov = PI / 2;

	while (!glfwWindowShouldClose(window)) {
		camera_handle_inputs(&camera, keys);
		camera_to_view_matrix(camera, &view_matrix);
		glfwPollEvents();

		glClearColor(0.2, 0.8, 1., 0.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		perespective_projection_matrix(
			near,
			far,
			fov,
			aspect_ratio,
			&projection_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);
		glUseProgram(shader_program);
		glUniformMatrix4fv(
			projection_matrix_uniform,
			1,
			GL_TRUE,
			projection_matrix);
		glUniformMatrix4fv(
			view_matrix_uniform,
			1,
			GL_TRUE,
			view_matrix);
		glUniform1ui(tick_uniform, tick);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_texture);
		glBindVertexArray(vao);
		glDrawElements(
			GL_TRIANGLES,
			sizeof(BASE_BLOCK_INDICES) / sizeof(unsigned int),
			GL_UNSIGNED_INT,
			0);

		glfwSwapBuffers(window);
		tick++;
	}

	ret = 0;
exit:
	free(vert_shader);
	free(frag_shader);
	free(atlas_file);
	glfwTerminate();

	return ret;
}
