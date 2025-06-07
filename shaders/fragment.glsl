#version 460 core

in vec2 uv;
flat in int block_type;
out vec4 frag_color;

uniform sampler2D atlas_texture;

void main() {
	frag_color = texture(atlas_texture, vec2(0.5 * (block_type & 0xf), 0.5 * (block_type >> 1)) + uv / 2);
}
