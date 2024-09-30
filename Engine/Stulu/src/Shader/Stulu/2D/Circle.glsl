#SShader "2D/Circle"

#type vertex
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_localPos;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;

#include "Stulu/Scene.glsl"

layout(std140, binding = 1) uniform modelData
{
	mat4 normalMatrix;
	mat4 transform;
};

struct VertexInfo
{
    vec3 localPos;
    vec4 color;
    float thickness;
    float fade;
};
layout (location = 0) out VertexInfo Input;

void main() {
	Input.localPos = a_localPos;
	Input.color = a_color;
	Input.thickness = a_thickness;
	Input.fade = a_fade;
	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
#type fragment

struct VertexInfo
{
    vec3 localPos;
    vec4 color;
    float thickness;
    float fade;
};
layout (location = 0) in VertexInfo Input;

layout (location = 0) out vec4 a_color;


void main() {
	 // Calculate distance and fill circle with white
    float distance = 1.0 - length(Input.localPos);
    float circle = smoothstep(0.0, Input.fade, distance);
    circle *= smoothstep(Input.thickness + Input.fade, Input.thickness, distance);

    // Set output color
    a_color = Input.color;
	a_color.a *= circle;

	if (circle == 0.0 || Input.color == vec4(0.0))
		discard;
}