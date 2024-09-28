#include "Stulu/Default/Vertex.glsl"

#type fragment

#include "Stulu/PBR.glsl"
#include "Stulu/Out.glsl"

struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in vertInput vertex;