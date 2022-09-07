##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	vec4 cameraNearFar;
	mat4 transform;
};

layout (location = 0) out vec4 out_color;

void main() {
	out_color = a_color;

	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

layout (location = 0) in vec4 in_color;

void main() {
	a_color = in_color;
}