##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};

out vec4 v_color;

void main() {
	v_color = a_color;
	gl_Position = u_viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec4 v_color = vec4(1.0f);
uniform vec4 u_color = vec4(1.0f);

void main() {
	color = u_color * v_color;
}