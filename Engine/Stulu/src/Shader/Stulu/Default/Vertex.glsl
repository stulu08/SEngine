#type vertex
#include "Stulu/MeshLayout.glsl"
#include "Stulu/Buffer/CameraBuffer.glsl"
#include "Stulu/Buffer/DefaultModelBuffer.glsl"

struct VertexData
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) out VertexData vertex;
layout (location = 5) out flat uint ST_EntityID;

void main()
{
	vec4 position = vec4(0.0);
	vec3 normal = vec3(0.0);
	CalculateVertexPositionAndNormal(position, normal);

	vertex.worldPos  = position.xyz;
	vertex.normal    = normal;
	vertex.texCoords = a_texCoords;
	vertex.color     = a_color;

	gl_Position = viewProjection * position;

	ST_EntityID = GetEntityID();

}