#SShader "Gizmo/Cube"

#type vertex
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;

#include "Stulu/Scene.glsl"

layout (location = 0) out vec4 out_color;

void main() {
	out_color = a_color;
	gl_Position = viewProjection * vec4(a_pos, 1.0);
}

#type fragment
layout(location = 0) out vec4 a_color;
layout (location = 0) in vec4 in_color;

void main() {
	a_color = in_color;
}