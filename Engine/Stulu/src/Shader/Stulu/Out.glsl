#ifndef _STULU_OUT_GLSL_
#define _STULU_OUT_GLSL_

#include "Stulu/Internals.glsl"

layout (location = 0) out vec4 FragColor;

layout (location = 1) out uint OutEntityID;
void WriteEntityIDOut(uint id) {
	OutEntityID = id;
}
#ifndef ST_USE_ENTITY_ID
uint ST_EntityID = ENTITY_ID_NULL;
#endif



void WriteDefaultOut(vec3 color, float alpha) {
	FragColor = vec4(color, alpha);
	WriteEntityIDOut(ST_EntityID);

	if (alpha == 0.0) {
		discard;
	}
}
void WriteDefaultOut(vec4 color) {
	WriteDefaultOut(color.xyz, color.w);
}
void WriteDefaultOut(vec3 color) {
	WriteDefaultOut(color, 1.0);
}
#ifdef ST_PBR
void WriteDefaultOut(PBRResult result) {
	WriteDefaultOut(result.color, result.alpha);
}
#endif

#endif