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



#ifdef ST_PBR
void WriteDefaultOut(PBRResult result) {
	FragColor = vec4(result.color, result.alpha);
	WriteEntityIDOut(ST_EntityID);
}
#endif
void WriteDefaultOut(vec4 color) {
	FragColor = color;
	WriteEntityIDOut(ST_EntityID);
}
void WriteDefaultOut(vec3 color) {
	FragColor = vec4(color, 1.0);
	WriteEntityIDOut(ST_EntityID);
}
void WriteDefaultOut(vec3 color, float alpha) {
	FragColor = vec4(color, alpha);
	WriteEntityIDOut(ST_EntityID);
}
#endif