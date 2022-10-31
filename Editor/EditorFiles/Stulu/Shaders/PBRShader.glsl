##properity Color()			|albedo
##properity Color(hdr=true)	|emissionHDRColor
##properity Range(0.0,1.0)	|metallic
##properity Range(0.0,1.0)	|roughness
##properity Range(0.0,1.0)	|ao

#version 460
##include "Stulu/Default"

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
struct PBRResult { 
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	vec3 lightOut;
	vec3 normal;
	vec3 color;
	float alpha;
};
*/
void main (){
	PBRData data;
	vec4 a_albedo = albedo * vertex.color;
	
	a_albedo *= srgbToLin(texture(albedoMap, vertex.texCoords * textureTilling).rgba);
	data.albedo = a_albedo.rgb;
	data.alpha = filterAlpha(a_albedo.a, transparencyMode, alphaCutOff);
	
	data.emission = (emission.rgb * texture(emissionMap, vertex.texCoords * textureTilling).rgb) * emission.a;

	data.ao = max(ao, texture(aoMap, vertex.texCoords * textureTilling).r);
	data.metallic = max(metallic, texture(metallicMap, vertex.texCoords * textureTilling).r);
	data.roughness = max(roughness, texture(roughnessMap, vertex.texCoords * textureTilling).r);

	data.normal = getNormalFromMap(vertex.worldPos, vertex.texCoords * textureTilling, vertex.normal, normalMap);
	data.worldPos = vertex.worldPos;
	data.texCoords = vertex.texCoords * textureTilling;

	PBRResult result = ST_PBR(data);
	writeColorToDefaultOutBuffer(result);

	if(data.alpha == 0.0) {
		discard;
	}

}
//may need this
/*
#define ST_USER_TEXTURE_0 5
#define ST_USER_TEXTURE_1 6
#define ST_USER_TEXTURE_2 7
#define ST_USER_TEXTURE_3 8
#define ST_USER_TEXTURE_4 9
#define ST_USER_TEXTURE_5 10
#define ST_USER_TEXTURE_6 11
#define ST_USER_TEXTURE_7 12
#define ST_USER_TEXTURE_8 13
#define ST_USER_TEXTURE_9 14
#define ST_USER_TEXTURE_10 15

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
float linearizeDepth(float depth, float near, float far);
vec3 srgbToLin(vec3 color);
vec4 srgbToLin(vec4 color);
#ifdef ST_USING_DEFAULT_OUT
	//Use default out buffer, use this if you use "Stulu/Default" or "out vec4 FragColor", this will later becoma more important with diffrent shading techniques
	void writeColorToDefaultOutBuffer(vec4 _color);
	void writeColorToDefaultOutBuffer(PBRResult _color);
#endif

#define ST_USER_TEXTURE_0  ST_USER_TEXTURE_START +  0
#define ST_USER_TEXTURE_1  ST_USER_TEXTURE_START +  1
#define ST_USER_TEXTURE_2  ST_USER_TEXTURE_START +  2
#define ST_USER_TEXTURE_3  ST_USER_TEXTURE_START +  3
#define ST_USER_TEXTURE_4  ST_USER_TEXTURE_START +  4
#define ST_USER_TEXTURE_5  ST_USER_TEXTURE_START +  5
#define ST_USER_TEXTURE_6  ST_USER_TEXTURE_START +  6
#define ST_USER_TEXTURE_7  ST_USER_TEXTURE_START +  7
#define ST_USER_TEXTURE_8  ST_USER_TEXTURE_START +  8
#define ST_USER_TEXTURE_9  ST_USER_TEXTURE_START +  9
#define ST_USER_TEXTURE_10 ST_USER_TEXTURE_START + 10
#define ST_USER_TEXTURE_START 
#define ST_USER_TEXTURE_END   
#define ST_USER_TEXTURE_COUNT ST_USER_TEXTURE_END - ST_USER_TEXTURE_START

const int st_maxLights = 50;
struct Light{
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};

layout(std140, binding = 0) uniform cameraData
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	vec4 cameraNearFar;
};
//			depends on renderer
// --------------------------------------------
// following is used by the default renderer:
//layout(std140, binding = 1) uniform modelData
//{
//		mat4 normalMatrix;
//		mat4 transform;
//};
// --------------------------------------------
// following is used by post processing:
//layout(std140, binding = 1) uniform modelData
//{
//		//float z;
//};
// --------------------------------------------
// following is used by post gizmo instancing
//struct InstanceData{
//	mat4 transforms;
//	vec4 instanceColors;
//};
//layout(std140, binding = 1) uniform modelDatayout(std140, binding = 1) uniform modelData
//{
//		InstanceData instanceData[180];
//};
// --------------------------------------------
layout(std140, binding = 2) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
layout(std140, binding = 3) uniform postProcessing
{
	float time;
	float delta;
	float toneMappingExposure;
	float gamma;
};
layout(std140, binding = 4) uniform shaderSceneData
{
	mat4 skyBoxRotation;
	vec4 clearColor;
	bool useSkybox;
	uint skyboxMapType;
	uint viewFlags;
	float env_lod;
};
layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;
layout (binding = 4) uniform sampler2D shadowMap;

struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in vertInput vertex;
*/