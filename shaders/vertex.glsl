#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

uniform float rotation;

out vec4 vert_color;

void main() {
	vec3 rotated_pos;
	mat3 rotation_matrix;

	rotation_matrix[0] = vec3(cos(rotation), 0.0, sin(rotation));
	rotation_matrix[1] = vec3(0.0, 1.0, 0.0);
	rotation_matrix[2] = vec3(-sin(rotation), 0.0, cos(rotation));
	rotated_pos = rotation_matrix * in_pos;
	gl_Position = vec4(rotated_pos.x, rotated_pos.y, rotated_pos.z, 1.0);
	vert_color = vec4(in_color, 1.0);
}
