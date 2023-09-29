##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_textureIndex;
layout (location = 3) in vec4 a_color;
layout (location = 4) in vec4 a_material;
##include "Stulu/Bindings"

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
layout (location = 1) out vertOutput _output;

void main()
{
	vec4 world = transform * vec4(a_pos, 1.0);

    textureIndex = a_textureIndex.x; 
    _output.worldPos = world.xyz;
    _output.normal = (normalMatrix * vec4(a_normal, 0.0)).xyz;
	_output.color = a_color;
	_output.texCoords = a_pos.xz;
	_output.roughness = a_material.x;
	_output.metallic = a_material.y;

    gl_Position = viewProjection * world;
}
##type fragment
#version 460 core
out vec4 FragColor;
#define ST_USING_DEFAULT_OUT 1

##include "Stulu/Functions"
##include "Stulu/Bindings"
##include "Stulu/PBR"
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

	PBRResult result = ST_PBR(data);
	writeColorToDefaultOutBuffer(result);

	if(data.alpha == 0.0) {
		discard;
	}

}