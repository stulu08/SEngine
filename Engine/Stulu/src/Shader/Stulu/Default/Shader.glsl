#include "Stulu/Default/Vertex.glsl"

#type fragment

#include "Stulu/PBR.glsl"
#include "Stulu/Out.glsl"

struct VertexData
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in VertexData vertex;