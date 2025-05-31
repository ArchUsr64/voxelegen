#version 330 core

in vec2 uv;
out vec4 frag_color;

uniform sampler2D atlas_texture;
uniform int block_type;

void main() {
	frag_color = texture(atlas_texture, vec2(0.5 * (block_type & 0xf), 0.5 * (block_type >> 1)) + uv / 2);
}
