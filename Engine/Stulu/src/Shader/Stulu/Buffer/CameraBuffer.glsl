#ifndef _STULU_CAMERA_BUFFER_GLSL_
#define _STULU_CAMERA_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout(std140, binding = ST_BUFFER_CAMERA_BIND) uniform CameraBufferData
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	vec4 cameraNearFar;
};

#endif