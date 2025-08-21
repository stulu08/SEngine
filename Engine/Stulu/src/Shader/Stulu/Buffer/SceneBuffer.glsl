#ifndef _STULU_SCENE_BUFFER_GLSL_
#define _STULU_SCENE_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout (binding = ST_SKYBOX_TEXTURE_BIND_ENV) uniform samplerCube environmentMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_IRR) uniform samplerCube irradianceMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_PRE) uniform samplerCube prefilterMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_BRD) uniform sampler2D BRDFLUTMap;

layout(std140, binding = ST_BUFFER_SCENE_BIND) uniform ShaderSceneBufferData
{
	mat4 skyBoxRotation;
	vec4 clearColor;

	bool useSkybox;
	uint viewFlags;
	float environmentStrength;
	float environmentReflections;

	mat4 lightSpaceMatrices[ST_MAX_SHADOW_CASCADES + 1];
	vec4 cascadePlaneDistances[ST_MAX_SHADOW_CASCADES + 1]; // distance (far plane), NearPlane
	vec4 shadowSettingsPack1; // BlendDistance, Poisson Quality, CascadeCount, LightWidth
	vec4 shadowSettingsPack2; // SoftShadows, PCSS Poisson Quality

	vec4 fogColor;
	float fogMode;
	float linearFogStart;
	float linearFogEnd;
	float fogDensity;
	float FogHorizonStrength;
	float FogHorizonFalloff;
	float EnableFogHorizon;
	float FogGroundStrength;
	float EnableGroundFog;
	float FogHorizonOffset;
	float FogHorizonHeightFalloff;
	float EnableHorizonHeightFog;
};


float CascadeBlendDistance = shadowSettingsPack1.x;
int ShadowQuality = int(shadowSettingsPack1.y);
int CascadeCount = int(shadowSettingsPack1.z);
float CascadeLightSize = shadowSettingsPack1.w;
float CascadeSoftShadows = shadowSettingsPack2.x;
int PCSSQuality = int(shadowSettingsPack2.y);
float CascadeBiasMod = shadowSettingsPack2.z;
#endif