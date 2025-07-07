#ifndef _STULU_POST_RPOCESS_BUFFER_GLSL_
#define _STULU_POST_RPOCESS_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout(std140, binding = ST_BUFFER_POSTPROCESS_BIND) uniform postProcessing
{
	float time;
	float delta;
	float enableGammaCorrection;
	float toneMappingExposure;
	float gamma;
	float bloomStrength;
};
#endif