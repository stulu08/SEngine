##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_localPos;
layout(location = 2) in vec4 a_color;

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};

struct VertexOutput
{
	vec3 localPos;
	vec4 color;
};

layout (location = 0) out VertexOutput Output;

void main() {
	Output.localPos = a_localPos;
	Output.color = a_color;
	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

struct VertexInput
{
	vec3 localPos;
	vec4 color;
};
layout (location = 0) in VertexInput Input;


void main() {

    //float distance = 1.0 - length(Input.localPos);

	float distanceToCenter = 1-distance(vec3(.0f),Input.localPos);

	float sphere = smoothstep(0.0, .5, distanceToCenter);

	if (sphere < 0.5)
		discard;

	a_color = vec4(vec3(1),1);
	//a_color.a *= sphere;

}