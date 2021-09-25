##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;

uniform mat4 u_viewProjection;

out vec4 v_color;

void main() {
	gl_Position = u_viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

uniform vec4 u_color = vec4(1.0f);

void main() {
	color = u_color;
}