#SShader "2D/Line"

#type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;

#include "Stulu/Scene.glsl"

layout (location = 0) out vec4 color;

void main() {
	color = a_color;
	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
#type fragment
#version 460 core

layout(location = 0) out vec4 a_color;
layout (location = 0) in vec4 color;

void main() {
	a_color = color;
}