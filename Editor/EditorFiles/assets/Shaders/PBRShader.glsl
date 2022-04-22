##properity Color()			|albedo
##properity Range(0.0,1.0)	|metallic
##properity Range(0.0,1.0)	|roughness
##properity Range(0.0,1.0)	|ao
##properity Range(0.0,1.0)	|alphaCutOff
##properity Enum(Opaque,Cutout,Transparent) |transparencyMode

#version 460
##add ST_default

layout(std140, binding = 3) uniform material {
	vec4  albedo;
	float metallic;
	float roughness;
	float ao;
	uint has_albedoMap;
	uint has_metallicMap;
	uint has_roughnessMap;
	uint has_normalMap;
	uint has_aoMap;
	vec2 textureTilling;
	uint transparencyMode;
	float alphaCutOff;
	uint useGLTFMetallicRoughnessMap;
};

layout(binding = 4) uniform sampler2D albedoMap;
layout(binding = 5) uniform sampler2D metallicMap;
layout(binding = 6) uniform sampler2D roughnessMap;
layout(binding = 7) uniform sampler2D normalMap;
layout(binding = 8) uniform sampler2D aoMap;

void main (){
	PBRData data;
	vec4 a_albedo = albedo;
	data.metallic = metallic;
	data.roughness = roughness;
	data.ao = ao;

	
	if(has_albedoMap == 1)
		a_albedo *= pow(texture(albedoMap, vertex.texCoords * textureTilling).rgba, vec4(vec3(2.2),1.0));

	data.albedo = a_albedo.rgb;
	if(useGLTFMetallicRoughnessMap == 0){
		if(has_metallicMap == 1)
			data.metallic = texture(metallicMap, vertex.texCoords * textureTilling).r;
		if(has_roughnessMap == 1)
			data.roughness = texture(roughnessMap, vertex.texCoords * textureTilling).g;
	}else{
		if(has_metallicMap == 1)
			data.metallic = texture(metallicMap, vertex.texCoords * textureTilling).r;
		if(has_roughnessMap == 1)
			data.roughness = texture(metallicMap, vertex.texCoords * textureTilling).g;
	}
	if(has_aoMap == 1)
		data.ao = texture(aoMap, vertex.texCoords * textureTilling).r;

	if(has_normalMap == 1)
		data.normal = getNormalFromMap(vertex.worldPos, vertex.texCoords * textureTilling, vertex.normal, normalMap);
	else
		data.normal = normalize(vertex.normal);

	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords;

	data.cameraPosition = vec3(cameraPosition);
	data.cameraRotation = vec3(cameraRotation);

	data.toneMappingExposure = toneMappingExposure;
	data.gamma = gamma;
	data.env_lod = env_lod;

	data.useSkybox = useSkybox == 1;
	data.toneMappingAndGammaCorrection = true;

	data.lights = lights;
	data.lightCount = lightCount;
	data.useLights = true;

	//CutOut
	if(transparencyMode == 1) {
		if(a_albedo.a <= alphaCutOff){
			data.alpha = 0.0;//full transparent
		}else{
			data.alpha = 1.0;//full opaque
		}
	}
	//Transparent 
	else if(transparencyMode == 2) {
		data.alpha = a_albedo.a;//keep alpha
	}
	//Opaque
	else{
		data.alpha = 1.0f;//ignore alpha
	}

	FragColor = ST_pbr_calculation(data);
}

//may need this
/*
const float PI = 3.14159265359;
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 getNormalFromMap(vec3 world, vec2 tex, vec3 normal, sampler2D map);

const int st_maxLights = 25;
struct Light{
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};
struct PBRData {
	vec3  albedo;
	float metallic ;
	float roughness;
	float ao;
	float alpha;

	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;

	vec3 cameraPosition;
	vec3 cameraRotation;

	float toneMappingExposure;
	float gamma;
	float env_lod;

	bool toneMappingAndGammaCorrection;
	bool useSkybox;

	Light lights[st_maxLights];
	uint lightCount;
	bool useLights;
};
layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
layout(std140, binding = 2) uniform postProcessing
{
	float toneMappingExposure;
	float gamma;
	uint useSkybox;
};
struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
layout (location = 0) in vertInput vertex;
*/