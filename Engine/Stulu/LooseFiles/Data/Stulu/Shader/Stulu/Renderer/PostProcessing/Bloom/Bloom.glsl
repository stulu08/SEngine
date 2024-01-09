#ifndef _STULU_BLOOM_GLSL_
#define _STULU_BLOOM_GLSL_

vec4 apply_bloom(vec4 original, const float strength, const vec2 tex, sampler2D bloomMap) {
	vec3 bc = texture(bloomMap, tex).rgb;
	original = vec4((original.rgb + (bc * strength)), original.a);
	return original;
}

#endif