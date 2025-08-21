#ifndef _STULU_DEFAULT_MATERIAL_BUFFER_GLSL_
#define _STULU_DEFAULT_MATERIAL_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

layout(std140, binding = ST_BUFFER_MATERIAL_BIND) uniform InternalMaterialData {
	uint ST_AlphaMode;
	float ST_AlphaCutOff;
};

#endif