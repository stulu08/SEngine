#SShader "Default/PBR"

#Expose Sampler2D("Albedo Map", auto_align, 0, White)
#Expose Sampler2D("Mask Map", auto_align, 1, Black)
#Expose Sampler2D("Metallic Map", auto_align, 2, Black)
#Expose Sampler2D("Roughness Map", auto_align, 3, Black)
#Expose Sampler2D("Normal Map", auto_align, 4, Black)
#Expose Sampler2D("Ambient Occlusion Map", auto_align, 5, Black)
#Expose Sampler2D("Emission Map", auto_align, 6, Black)

#Expose Color("Albedo", auto_align)
#Expose Float("Metallic", auto_align, 0, 1)
#Expose Float("Roughness", auto_align, 0, 1)
#Expose Float("Ambient Occlusion", auto_align, 0, 1)
#Expose Color("Emission", auto_align, true)
#Expose Float2("Tilling", auto_align)

#include "Stulu/Default/Shader.glsl"

layout(std140, binding = ST_USER_MATERIAL_BINDING) uniform Material {
	float hasAlbedoMap;
	float hasMaskMap;
	float hasMetallicMap;
	float hasRoughnessMap;
	float hasNormalMap;
	float hasAOMap;
	float hasEmissionMap;

	vec4 albedo;
	float metallic;
	float roughness;
	float ao;
	vec4 emission;
	vec2 textureTilling;
};

layout(binding = ST_USER_TEXTURE_0) uniform sampler2D albedoMap;
layout(binding = ST_USER_TEXTURE_1) uniform sampler2D maskMap;
layout(binding = ST_USER_TEXTURE_2) uniform sampler2D metallicMap;
layout(binding = ST_USER_TEXTURE_3) uniform sampler2D roughnessMap;
layout(binding = ST_USER_TEXTURE_4) uniform sampler2D normalMap;
layout(binding = ST_USER_TEXTURE_5) uniform sampler2D aoMap;
layout(binding = ST_USER_TEXTURE_6) uniform sampler2D emissionMap;

void main (){
	PBRData data;
	data.texCoords = vertex.texCoords * textureTilling;
	data.worldPos = vertex.worldPos;

	data.albedo = SampleColorTexture(albedoMap, data.texCoords, hasAlbedoMap, albedo * vertex.color);
	data.albedo.a = SampleTexture(maskMap, data.texCoords, hasMaskMap, FilterAlpha(data.albedo.a));

	data.emission = SampleColorTexture(emissionMap, data.texCoords, hasEmissionMap, emission.rgb) * emission.a;
	data.metallic = SampleTexture(metallicMap, data.texCoords, hasMetallicMap, metallic);
	data.ao = SampleTexture(aoMap, data.texCoords, hasAOMap, ao);
	data.roughness = SampleTexture(roughnessMap, data.texCoords, hasRoughnessMap, roughness);
	data.normal = SampleNormalMap(vertex.worldPos, data.texCoords, vertex.normal, normalMap, hasNormalMap);

	PBRResult result = ComputePBR(data);

	ApplyDefaultFog(result.color, vertex.worldPos);

	WriteDefaultOut(result);

	if (data.albedo.a == 0.0) {
		discard;
	}

}