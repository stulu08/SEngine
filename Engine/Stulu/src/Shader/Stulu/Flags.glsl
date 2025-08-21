#ifndef _STULU_FLAGS_GLSL_
#define _STULU_FLAGS_GLSL_

const uint ShaderViewFlag_DisplayLighting			= 0x00000001u;
const uint ShaderViewFlag_DisplayDiffuse			= 0x00000002u;
const uint ShaderViewFlag_DisplaySpecular			= 0x00000004u;
const uint ShaderViewFlag_DisplayNormal				= 0x00000008u;
const uint ShaderViewFlag_DisplayRoughness			= 0x00000010u;
const uint ShaderViewFlag_DisplayMetallic			= 0x00000020u;
const uint ShaderViewFlag_DisplayAmbient			= 0x00000040u;
const uint ShaderViewFlag_DisplayTexCoords			= 0x00000080u;
const uint ShaderViewFlag_DisplayVertices			= 0x00000100u;
const uint ShaderViewFlag_DisplayEmission			= 0x00000200u;
const uint ShaderViewFlag_DisplayAmbientOcclusion	= 0x00000400u;
const uint ShaderViewFlag_DisplayDepth				= 0x00000800u;

bool isFlagEnabled(uint flags, uint flag) {
	return (flags & flag) == flag;
}
bool isFlagDisabled(uint flags, uint flag) {
	return (flags & flag) != flag;
}
#endif