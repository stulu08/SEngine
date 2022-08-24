##properity Color()			|albedo
##properity Color(hdr=true)	|emissionHDRColor
##properity Range(0.0,1.0)	|metallic
##properity Range(0.0,1.0)	|roughness
##properity Range(0.0,1.0)	|ao

#version 460
##add ST_default
layout(std140, binding = 4) uniform material {
	uint transparencyMode;
	float alphaCutOff;
	vec4  albedo;
	float metallic;
	float roughness;
	float ao;
	vec4 emission;
	vec2 textureTilling;
};
layout(binding = 4) uniform sampler2D albedoMap;
layout(binding = 5) uniform sampler2D metallicMap;
layout(binding = 6) uniform sampler2D roughnessMap;
layout(binding = 7) uniform sampler2D normalMap;
layout(binding = 8) uniform sampler2D aoMap;
layout(binding = 9) uniform sampler2D emissionMap;
/*
struct PBRData {
	vec3  albedo;
	vec3  emission;
	float metallic;
	float roughness;
	float ao;
	float alpha;

	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
*/
void main (){
	PBRData data;
	vec4 a_albedo = albedo * vertex.color;
	
	a_albedo *= srgbToLin(texture(albedoMap, vertex.texCoords * textureTilling).rgba);
	data.albedo = a_albedo.rgb;
	data.alpha = filterAlpha(a_albedo.a, transparencyMode, alphaCutOff);
	if(data.alpha == 0.0)
		discard;

	data.emission = (emission.rgb * texture(emissionMap, vertex.texCoords * textureTilling).rgb) * emission.a;

	data.ao = max(ao, texture(aoMap, vertex.texCoords * textureTilling).r);
	data.metallic = max(metallic, texture(metallicMap, vertex.texCoords * textureTilling).r);
	data.roughness = max(roughness, texture(metallicMap, vertex.texCoords * textureTilling).g);

	data.normal = getNormalFromMap(vertex.worldPos, vertex.texCoords * textureTilling, vertex.normal, normalMap);

	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords * textureTilling;

	
	FragColor = ST_pbr_calculation(data);
}

//may need this
/*
const uint ShaderViewFlag_DisplayLighting	= 0x00000001u;
const uint ShaderViewFlag_DisplayDiffuse	= 0x00000002u;
const uint ShaderViewFlag_DisplaySpecular	= 0x00000004u;
const uint ShaderViewFlag_DisplayNormal		= 0x00000008u;
const uint ShaderViewFlag_DisplayRoughness	= 0x00000010u;
const uint ShaderViewFlag_DisplayMetallic	= 0x00000020u;
const uint ShaderViewFlag_DisplayAmbient	= 0x00000040u;
const uint ShaderViewFlag_DisplayTexCoords	= 0x00000080u;
const uint ShaderViewFlag_DisplayVertices	= 0x00000100u;
const uint ShaderViewFlag_DisplayEmission	= 0x00000200u;
const uint ShaderViewFlag_DisplayDepth		= 0x00000400u;
const float PI = 3.14159265359;


bool isFlagEnabled(uint flags, uint flag);
bool isFlagDisabled(uint flags, uint flag);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 getNormalFromMap(vec3 world, vec2 tex, vec3 normal, sampler2D map);
vec4 gammaCorrect(vec4 color, float _gamma, float exposure);
vec3 gammaCorrect(vec3 color, float _gamma, float exposure);
float filterAlpha(float alpha, uint mode, float cutOut = 1.0f);
vec3 srgbToLin(vec3 color);
vec4 srgbToLin(vec4 color);

const int st_maxLights = 25;
struct Light{
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
layout(std140, binding = 1) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
layout(std140, binding = 2) uniform postProcessing
{
	float time;
	float delta;
};
layout(std140, binding = 3) uniform shaderSceneData
{
	float toneMappingExposure;
	float gamma;
	float env_lod;
	bool enableGammaCorrection;
	vec4 clearColor;
	bool useSkybox;
	uint skyboxMapType;
	uint viewFlags;
};
layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;

struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in vertInput vertex;
*/