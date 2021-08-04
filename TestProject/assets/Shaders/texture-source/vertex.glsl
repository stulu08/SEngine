#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
uniform mat4 u_viewProjection;
uniform mat4 u_transform;
out vec2 v_texCoord;
void main() {
	v_texCoord = a_texCoord;
	gl_Position = u_viewProjection * u_transform * vec4(a_pos, 1.0);
}