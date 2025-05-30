#ifndef _STULU_SCENE_GLSL_
#define _STULU_SCENE_GLSL_
#include "Stulu/Internals.glsl"

layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;
layout (binding = 4) uniform sampler2D shadowMap;

layout(std140, binding = 0) uniform cameraData
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	vec4 cameraNearFar;
};
layout(std140, binding = 4) uniform shaderSceneData
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
};
/*
			depends on renderer
 --------------------------------------------
 following is used by the default renderer:
layout(std140, binding = 1) uniform modelData
{
		mat4 normalMatrix;
		mat4 transform;
};
 --------------------------------------------
 following is used by post processing:
layout(std140, binding = 1) uniform modelData
{
		//float z;
};
 --------------------------------------------
 following is used by post gizmo instancing
struct InstanceData{
	mat4 transforms;
	vec4 instanceColors;
};
layout(std140, binding = 1) uniform modelDatayout(std140, binding = 1) uniform modelData
{
		InstanceData instanceData[ST_MAX_INSTANCES];
};
 --------------------------------------------
 */
#endif