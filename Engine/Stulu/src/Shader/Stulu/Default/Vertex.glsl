#type vertex

#include "Stulu/MeshLayout.glsl"
#include "Stulu/Scene.glsl"

layout(std140, binding = 1) uniform modelData
{
	mat4 normalMatrix;
	mat4 transform;
};
struct VertexData
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) out VertexData vertex;

void main()
{
	vec4 world = transform * vec4(a_pos, 1.0);

    vertex.worldPos = world.xyz;
    vertex.normal = (normalMatrix * vec4(a_normal, 0.0)).xyz;
    vertex.texCoords = a_texCoords;
	vertex.color = a_color;

    gl_Position = viewProjection * world;
}