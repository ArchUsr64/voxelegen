#version 460 core

in vec3 in_pos;
in vec2 in_uv;

uniform vec2 rotation;
uniform mat4 projection_matrix;

out vec2 uv;

void main() {
	vec4 out_pos;
	vec3 rotated_pos;
	mat3 rotation_matrix_x, rotation_matrix_y;

	mat4 translation_matrix;
	translation_matrix[0] = vec4(1.0, 0.0, 0.0, 0.0);
	translation_matrix[1] = vec4(0.0, 1.0, 0.0, 0.0);
	translation_matrix[2] = vec4(0.0, 0.0, 1.0, 2.0);
	translation_matrix[3] = vec4(0.0, 0.0, 0.0, 1.0);

	uv = in_uv;

	rotation_matrix_x[0] = vec3(1.0, 0.0, 0.0);
	rotation_matrix_x[1] = vec3(0.0, cos(rotation.y), -sin(rotation.y));
	rotation_matrix_x[2] = vec3(0.0, sin(rotation.y), cos(rotation.y));

	rotation_matrix_y[0] = vec3(cos(rotation.x), 0.0, sin(rotation.x));
	rotation_matrix_y[1] = vec3(0.0, 1.0, 0.0);
	rotation_matrix_y[2] = vec3(-sin(rotation.x), 0.0, cos(rotation.x));

	rotated_pos = rotation_matrix_y * in_pos;
	rotated_pos *= rotation_matrix_x;
	out_pos = vec4(rotated_pos, 1.0);
	out_pos *= translation_matrix;
	gl_Position = projection_matrix * out_pos;
}
