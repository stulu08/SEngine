#ifndef _STULU_FULLSCREEEN_QUAD_BUFFER_GLSL_
#define _STULU_FULLSCREEEN_QUAD_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout(std140, binding = ST_BUFFER_MODEL_BIND) uniform FullScreenQuadBuffer
{
	float QuadZPos;
	uint pixelWidth;
	uint pixelHeight;
};

#endif