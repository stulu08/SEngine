#SShader "Default/PBR"

#Expose Sampler2D("Albedo Map", auto_align, 0, White)
#Expose Sampler2D("Metallic Map", auto_align, 1, Black)
#Expose Sampler2D("Roughness Map", auto_align, 2, Black)
#Expose Sampler2D("Normal Map", auto_align, 3, Black)
#Expose Sampler2D("Ambient Occlusion Map", auto_align, 4, Black)
#Expose Sampler2D("Emission Map", auto_align, 5, Black)

#Expose Color("Albedo", auto_align)
#Expose Float("Metallic", auto_align, 0, 1)
#Expose Float("Roughness", auto_align, 0, 1)
#Expose Float("Ambient Occlusion", auto_align, 0, 1)
#Expose Color("Emission", auto_align, true)
#Expose Float2("Tilling", auto_align)

#include "Stulu/Default/Shader.glsl"

layout(std140, binding = ST_USER_MATERIAL_BINDING) uniform Material {
	float hasAlbedoMap;
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
layout(binding = ST_USER_TEXTURE_1) uniform sampler2D metallicMap;
layout(binding = ST_USER_TEXTURE_2) uniform sampler2D roughnessMap;
layout(binding = ST_USER_TEXTURE_3) uniform sampler2D normalMap;
layout(binding = ST_USER_TEXTURE_4) uniform sampler2D aoMap;
layout(binding = ST_USER_TEXTURE_5) uniform sampler2D emissionMap;

void main (){
	PBRData data;
	vec4 a_albedo = albedo * vertex.color;

	// Conditionally sample albedo
	vec4 albedoTex = srgbToLin(texture(albedoMap, vertex.texCoords * textureTilling));
	a_albedo *= mix(vec4(1.0), albedoTex, hasAlbedoMap); // Use white if not bound
	data.albedo = a_albedo.rgb;
	data.alpha = FilterAlpha(a_albedo.a);

	// Conditionally sample emission
	vec3 emissionTex = texture(emissionMap, vertex.texCoords * textureTilling).rgb;
	data.emission = mix(vec3(0.0), emissionTex, hasEmissionMap) * emission.rgb * emission.a;

	// Conditionally sample AO
	float aoTex = texture(aoMap, vertex.texCoords * textureTilling).r;
	data.ao = mix(ao, max(ao, aoTex), hasAOMap);

	// Conditionally sample metallic
	float metallicTex = texture(metallicMap, vertex.texCoords * textureTilling).r;
	data.metallic = mix(metallic, max(metallic, metallicTex), hasMetallicMap);

	// Conditionally sample roughness
	float roughnessTex = texture(roughnessMap, vertex.texCoords * textureTilling).r;
	data.roughness = mix(roughness, max(roughness, roughnessTex), hasRoughnessMap);

	// Conditionally apply normal map
	vec3 normalMapped = getNormalFromMap(vertex.worldPos, vertex.texCoords * textureTilling, vertex.normal, normalMap);
	data.normal = mix(vertex.normal, normalMapped, hasNormalMap);

	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords * textureTilling;

	PBRResult result = ComputePBR(data);
	WriteDefaultOut(result);

	if (data.alpha == 0.0) {
		discard;
	}

}