##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
uniform vec3 u_worldPos;

out vec2 v_texCoord;

void main() {
	v_texCoord = a_texCoord;
	gl_Position = u_viewProjection * vec4(u_worldPos + a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
	color = texture(u_texture, v_texCoord);
}