#version 330 core

in vec2 uv;
out vec4 frag_color;

uniform sampler2D atlas_texture;

void main() {
	frag_color = texture(atlas_texture, uv);
}
