##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
out vec2 texCoords;
uniform mat4 u_transform;
void main() {
	texCoords = a_texCoords;
	gl_Position = u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

uniform sampler2D u_texture;

in vec2 texCoords;

void main() {
	a_color = texture(u_texture, texCoords);
}