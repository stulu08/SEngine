#ifndef _STULU_OUT_GLSL_
#define _STULU_OUT_GLSL_

layout (location = 0) out vec4 FragColor;

#ifdef ST_PBR
void WriteDefaultOut(PBRResult result) {
	FragColor = vec4(result.color, result.alpha);
}
#endif
void WriteDefaultOut(vec4 color) {
	FragColor = color;
}
void WriteDefaultOut(vec3 color) {
	FragColor = vec4(color, 1.0);
}
void WriteDefaultOut(vec3 color, float alpha) {
	FragColor = vec4(color, alpha);
}

#endif