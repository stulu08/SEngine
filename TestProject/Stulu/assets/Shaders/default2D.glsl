##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;

uniform mat4 u_viewProjection;

out vec2 v_texCoord;
out vec4 v_color;

void main() {
	v_texCoord = a_texCoord;
	v_color = a_color;
	gl_Position = u_viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2D u_texture;
uniform vec2 u_textureTiling = vec2(1.0f,1.0f);

void main() {

	//color = texture(u_texture, v_texCoord * u_textureTiling) * v_color;
	color = v_color;
}