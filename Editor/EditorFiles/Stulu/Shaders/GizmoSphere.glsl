##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_color;

struct InstanceData{
	mat4 transforms;
	vec4 instanceColors;
};
##include "Stulu/Bindings"

layout(std140, binding = 1) uniform modelData
{
	InstanceData instanceData[180];
};

struct VertexOutput
{
	vec4 color;
	vec4 instanceColor;
};

layout (location = 0) out VertexOutput Output;

void main() {
	Output.color = a_color;
	Output.instanceColor = instanceData[gl_InstanceID].instanceColors;
	gl_Position =  viewProjection * instanceData[gl_InstanceID].transforms * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

struct VertexInput
{
	vec4 color;
	vec4 instanceColor;
};
layout (location = 0) in VertexInput Input;


void main() {
	a_color = Input.color * Input.instanceColor;
}