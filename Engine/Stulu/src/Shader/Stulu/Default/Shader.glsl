#include "Stulu/Default/Vertex.glsl"

#type fragment
#define ST_USE_ENTITY_ID 1

struct VertexData
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in VertexData vertex;
layout (location = 5) in flat uint ST_EntityID;

#include "Stulu/PBR.glsl"
#include "Stulu/Out.glsl"
