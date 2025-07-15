#ifndef _STULU_SCENE_BUFFER_GLSL_
#define _STULU_SCENE_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout (binding = ST_SKYBOX_TEXTURE_BIND_ENV) uniform samplerCube environmentMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_IRR) uniform samplerCube irradianceMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_PRE) uniform samplerCube prefilterMap;
layout (binding = ST_SKYBOX_TEXTURE_BIND_BRD) uniform sampler2D BRDFLUTMap;
layout (binding = ST_SHADOW_TEXTURE_BIND_MAP) uniform sampler2DArray cascadeShadowMap;

layout(std140, binding = ST_BUFFER_SCENE_BIND) uniform ShaderSceneBufferData
{
	mat4 skyBoxRotation;
	vec4 clearColor;

	bool useSkybox;
	uint viewFlags;
	int shadowCaster;
	uint cascadeCount;

	mat4 lightSpaceMatrices[ST_MAX_SHADOW_CASCADES + 1];
	vec4 cascadePlaneDistances[ST_MAX_SHADOW_CASCADES + 1];
	vec4 CascadeBlendDistance;
	vec4 shadowCasterPosPacked; // shadowCasterPos (xyz) + env_lod (w)

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


vec3 shadowCasterPos = shadowCasterPosPacked.xyz;
float env_lod = shadowCasterPosPacked.w;

#endif