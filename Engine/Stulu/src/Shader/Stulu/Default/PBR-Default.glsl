#SShader "Default/PBR"

#properity Color()			| albedo
#properity Color(hdr=true)	| emissionHDRColor
#properity Range(0.0,1.0)	| metallic
#properity Range(0.0,1.0)	| roughness
#properity Range(0.0,1.0)	| ao

#include "Stulu/Default/Shader.glsl"

layout(std140, binding = 5) uniform material {
	uint transparencyMode;
	float alphaCutOff;
	vec4  albedo;
	float metallic;
	float roughness;
	float ao;
	vec4 emission;
	vec2 textureTilling;
};

layout(binding = ST_USER_TEXTURE_0) uniform sampler2D albedoMap;
layout(binding = ST_USER_TEXTURE_1) uniform sampler2D metallicMap;
layout(binding = ST_USER_TEXTURE_2) uniform sampler2D roughnessMap;
layout(binding = ST_USER_TEXTURE_3) uniform sampler2D normalMap;
layout(binding = ST_USER_TEXTURE_4) uniform sampler2D aoMap;
layout(binding = ST_USER_TEXTURE_5) uniform sampler2D emissionMap;

void main (){
	PBRData data;
	vec4 a_albedo = albedo * vertex.color;
	
	a_albedo *= srgbToLin(texture(albedoMap, vertex.texCoords * textureTilling).rgba);
	data.albedo = a_albedo.rgb;
	data.alpha = filterAlpha(a_albedo.a, transparencyMode, alphaCutOff);
	
	data.emission = texture(emissionMap, vertex.texCoords * textureTilling).rgb * emission.rgb * emission.a;

	data.ao = max(ao, texture(aoMap, vertex.texCoords * textureTilling).r);
	data.metallic = max(metallic, texture(metallicMap, vertex.texCoords * textureTilling).r);
	data.roughness = max(roughness, texture(roughnessMap, vertex.texCoords * textureTilling).r);

	data.normal = getNormalFromMap(vertex.worldPos, vertex.texCoords * textureTilling, vertex.normal, normalMap);
	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords * textureTilling;

	PBRResult result = ComputePBR(data);
	WriteDefaultOut(result);

	if(data.alpha == 0.0) {
		discard;
	}

}