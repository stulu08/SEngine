#ifndef _STULU_LIGHT_BUFFER_GLSL_
#define _STULU_LIGHT_BUFFER_GLSL_

#include "Stulu/Internals.glsl"

#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

layout (binding = ST_DEFAULT_SHADOW_TEXTURE_BIND_MAP) uniform sampler2D DefaultShadowMap;
layout (binding = ST_POINT_SHADOW_TEXTURE_BIND_MAP) uniform samplerCubeArray PointLightShadowMap;
layout (binding = ST_CASCADE_SHADOW_TEXTURE_BIND_MAP) uniform sampler2DArray CascadedShadowMap;

struct Light {
	vec4 ColorStrength;	// xyz = color, w = strength
	vec4 PositionType;	// xyz = position or irrelevant for directional, w = LightType enum
	vec4 Direction;		// xyz = direction (e.g. forward), w = unused
	vec4 Data;			// Light-specific data:
	// Directional:		x = size, y = isMainLight, z,w = 1
	// Point:			x = radius
	// Spot:			x = innerCutoff, y = outerCutoff
	vec4 ShadowData;	// x = Map Index, y = Bias, z = Strength, w = soft
};

layout(std140, binding = ST_BUFFER_LIGHT_BIND) uniform lightData
{
	Light lights[st_maxLights];
	uint lightCount;
};

int GetNumLights() {
	return int(lightCount);
}

// cascaded
bool IsLightMainCaster(int i) {
	return lights[i].Data.y == 1.0 && lights[i].PositionType.w == LIGHT_TYPE_DIR;
}

#endif