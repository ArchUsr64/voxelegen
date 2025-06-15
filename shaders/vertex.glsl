#version 460 core

in vec3 in_pos;
in vec2 in_uv;

uniform vec3 chunk_pos;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform uint tick;

out vec2 uv;

void main() {
	vec4 out_pos;
	vec3 rotated_pos;
	mat4 translation_matrix;

	translation_matrix[0] = vec4(1.0, 0.0, 0.0, chunk_pos.x);
	translation_matrix[1] = vec4(0.0, 1.0, 0.0, chunk_pos.y);
	translation_matrix[2] = vec4(0.0, 0.0, 1.0, chunk_pos.z);
	translation_matrix[3] = vec4(0.0, 0.0, 0.0, 1.0);

	uv = in_uv;

	out_pos = vec4(in_pos, 1.0);
	out_pos *= translation_matrix;
	gl_Position = projection_matrix * inverse(view_matrix) * out_pos;
}
