#ifndef _STULU_LIGHT_BUFFER_GLSL_
#define _STULU_LIGHT_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

struct Light {
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};


layout(std140, binding = ST_BUFFER_LIGHT_BIND) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};

#endif