#type vertex
#version 460 core

#include "Stulu/MeshLayout.glsl"
#include "Stulu/Scene.glsl"

layout(std140, binding = 1) uniform modelData
{
	mat4 normalMatrix;
	mat4 transform;
};
struct vertOutput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) out vertOutput v_output;

void main()
{
	vec4 world = transform * vec4(a_pos, 1.0);

    v_output.worldPos = world.xyz;
    v_output.normal = (normalMatrix * vec4(a_normal, 0.0)).xyz;
    v_output.texCoords = a_texCoords;
	v_output.color = a_color;

    gl_Position = viewProjection * world;
}