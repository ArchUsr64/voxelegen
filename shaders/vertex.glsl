#version 330 core

in vec3 in_pos;
in vec3 in_color;

uniform vec2 rotation;

out vec4 vert_color;

void main() {
	vec3 rotated_pos;
	mat3 rotation_matrix_x, rotation_matrix_y;

	rotation_matrix_x[0] = vec3(1.0, 0.0, 0.0);
	rotation_matrix_x[1] = vec3(0.0, cos(rotation.y), -sin(rotation.y));
	rotation_matrix_x[2] = vec3(0.0, sin(rotation.y), cos(rotation.y));

	rotation_matrix_y[0] = vec3(cos(rotation.x), 0.0, sin(rotation.x));
	rotation_matrix_y[1] = vec3(0.0, 1.0, 0.0);
	rotation_matrix_y[2] = vec3(-sin(rotation.x), 0.0, cos(rotation.x));

	rotated_pos = rotation_matrix_x * rotation_matrix_y * in_pos;
	gl_Position = vec4(rotated_pos.x, rotated_pos.y, rotated_pos.z, 1.0);
	vert_color = vec4(in_color, 1.0);
}
