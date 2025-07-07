#ifndef _STULU_POSTPROCESSING_GLSL_
#define _STULU_POSTPROCESSING_GLSL_

#include "Stulu/Buffer/PostProcessBuffer.glsl"

vec4 gammaCorrect(vec4 color, const float _gamma, const float exposure) {
	color = vec4(vec3(1.0) - exp(-color.rgb * exposure),color.a);//HDR tonemapping
	color = vec4(pow(color.rgb, vec3(1.0/_gamma)),color.a); //gamma correct
	return color;
}
vec3 gammaCorrect(vec3 color, const float _gamma, const float exposure) {
	color = vec3(1.0) - exp(-color.rgb * exposure);//HDR tonemapping
	color = pow(color.rgb, vec3(1.0/_gamma)); //gamma correct
	return color;
}
vec4 gammaCorrect(vec4 color) {
	return gammaCorrect(color, gamma, toneMappingExposure);
}
vec3 gammaCorrect(vec3 color) {
	return gammaCorrect(color, gamma, toneMappingExposure);
}
#include "Stulu/Renderer/PostProcessing/Bloom/Bloom.glsl"

#endif