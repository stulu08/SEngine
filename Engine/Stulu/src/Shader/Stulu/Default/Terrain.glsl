#SShader "Default/Terrain"

#type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_textureIndex;
layout (location = 3) in vec4 a_color;
layout (location = 4) in vec4 a_material;

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
	float roughness;
	float metallic;
};
layout (location = 0) out flat float textureIndex;
layout (location = 1) out vertOutput v_output;

void main()
{
	vec4 world = transform * vec4(a_pos, 1.0);

    textureIndex = a_textureIndex.x; 
    v_output.worldPos = world.xyz;
    v_output.normal = (normalMatrix * vec4(a_normal, 0.0)).xyz;
	v_output.color = a_color;
	v_output.texCoords = a_pos.xz;
	v_output.roughness = a_material.x;
	v_output.metallic = a_material.y;

    gl_Position = viewProjection * world;
}


#type fragment
#version 460 core

#include "Stulu/PBR.glsl"
#include "Stulu/Out.glsl"

struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
	float roughness;
	float metallic;
};
layout (location = 0) in flat float textureIndex;
layout (location = 1) in vertInput vertex;

layout(std140, binding = 5) uniform material {
	uint transparencyMode;
	float alphaCutOff;
	vec2 size;
	float brushWorldPos;
};
layout (binding = ST_USER_TEXTURE_START) uniform sampler2D u_textures[ST_USER_TEXTURE_COUNT];

void main (){
	PBRData data;
	
	data.albedo = vertex.color.rgb;
	data.emission = vec3(0.0);
	data.ao = 0.2;
	data.metallic = vertex.metallic;
	data.roughness = vertex.roughness;
	data.alpha = vertex.color.a;

	data.normal = vertex.normal;
	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords;

	PBRResult result = ComputePBR(data);
	WriteDefaultOut(result);

	if(data.alpha == 0.0) {
		discard;
	}
}