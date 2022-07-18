##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_localPos;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;

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
	float thickness;
	float fade;
};

layout (location = 0) out VertexOutput Output;

void main() {
	Output.localPos = a_localPos;
	Output.color = a_color;
	Output.thickness = a_thickness;
	Output.fade = a_fade;
	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

struct VertexInput
{
	vec3 localPos;
	vec4 color;
	float thickness;
	float fade;
};
layout (location = 0) in VertexInput Input;


void main() {
	 // Calculate distance and fill circle with white
    float distance = 1.0 - length(Input.localPos);
    float circle = smoothstep(0.0, Input.fade, distance);
    circle *= smoothstep(Input.thickness + Input.fade, Input.thickness, distance);

	if (circle == 0.0)
		discard;

    // Set output color
    a_color = Input.color;
	a_color.a *= circle;
}