#version 460 core
layout(location = 0) out vec4 color;
in vec2 v_texCoord;
uniform sampler2D u_texture;
void main() {
	color = texture(u_texture, v_texCoord);
}