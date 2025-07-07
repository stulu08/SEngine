#ifndef _STULU_SCENE_BUFFER_GLSL_
#define _STULU_SCENE_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout (binding = ST_SKYBOX_TEXTURE_BIND_ENV) uniform samplerCube environmentMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_IRR) uniform samplerCube irradianceMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_PRE) uniform samplerCube prefilterMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_BRD) uniform sampler2D BRDFLUTMap;
layout (binding = ST_SHADOW_TEXTURE_BIND_MAP) uniform sampler2D shadowMap;

layout(std140, binding = ST_BUFFER_SCENE_BIND) uniform ShaderSceneBufferData
{
	mat4 skyBoxRotation;
	vec4 clearColor;

	bool useSkybox;
	uint emptyData;
	uint viewFlags;
	float env_lod;

	mat4 lightSpaceMatrix;

	vec3 shadowCasterPos;
	int shadowCaster;

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
#endif