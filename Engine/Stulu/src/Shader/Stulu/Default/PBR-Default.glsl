#SShader "Default/PBR"

#define auto_align 0
#define Sampler2D(name, slot, fallback = [Black : White])
#define Color(name, offset, isHDR)
#define Float(name, offset, minVal, maxVal)
#define Float2(name, offset)
#define Float4(name, offset)

#Expose Sampler2D("Albedo Map", 0, White)
#Expose Sampler2D("Metallic Map", 1, Black)
#Expose Sampler2D("Roughness Map", 2, Black)
#Expose Sampler2D("Normal Map", 3, Black)
#Expose Sampler2D("Ambient Occlusion Map", 4, Black)
#Expose Sampler2D("Emission Map", 5, Black)

#Expose Color("Albedo", auto_align)
#Expose Float("Metallic", auto_align, 0, 1)
#Expose Float("Roughness", auto_align, 0, 1)
#Expose Float("Ambient Occlusion", auto_align, 0, 1)
#Expose Color("Emission", auto_align, true)
#Expose Float2("Tilling", auto_align)

#include "Stulu/Default/Shader.glsl"

layout(std140, binding = ST_USER_MATERIAL_BINDING) uniform Material {
	uint transparencyMode;
	float alphaCutOff;
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