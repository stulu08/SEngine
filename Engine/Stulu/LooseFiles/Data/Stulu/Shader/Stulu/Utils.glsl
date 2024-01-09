#ifndef _STULU_UTILS_GLSL_
#define _STULU_UTILS_GLSL_

#include "Stulu/Branchless.glsl"

vec3 getNormalFromMap(vec3 world, vec2 tex, vec3 normal, sampler2D map)
{
	vec3 textureValue = texture(map, tex).xyz;
	if(textureValue == vec3(0))
		return normal;
    vec3 tangentNormal = textureValue * 2.0 - 1.0;
	
    vec3 Q1  = dFdx(world);
    vec3 Q2  = dFdy(world);
    vec2 st1 = dFdx(tex);
    vec2 st2 = dFdy(tex);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
	
    return normalize(TBN * tangentNormal);
}

float filterAlpha(float alpha, uint mode, float cutOut = 1.0) {
    if(mode == 1) {
		if(alpha > cutOut){
			return 1.0;//full opaque
		}else{
			return 0.0;//full transparent
		}
	}
	else if(mode == 2) {
		return alpha;//keep alpha
	}
    else{
		return 1.0;//ignore alpha
	}

	float value = -1.0;
    value += 1.0 * float(when_eq(mode, 1)) * when_gt(alpha, cutOut);
    value += alpha * float(when_eq(mode, 2));
    value += 1.0 * float(when_neq(value, -1.0));
    return value;
}
vec3 getSkyBoxCoords(vec3 view, mat4 _skyBoxRotation) {
	return normalize(_skyBoxRotation * vec4(view, 0.0)).xyz;
}

float linearizeDepth(float depth, float near, float far) {
	float z = depth * 2.0 - 1.0; // back to NDC 
    return ((2.0 * near * far) / (far + near - z * (far - near)));
}

vec3 srgbToLin(vec3 color){
	return pow(color, vec3(2.2));
}

vec4 srgbToLin(vec4 color){
	return vec4(srgbToLin(color.xyz), color.a);
}
#endif