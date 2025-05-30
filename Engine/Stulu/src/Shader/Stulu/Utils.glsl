#ifndef _STULU_UTILS_GLSL_
#define _STULU_UTILS_GLSL_

#include "Stulu/Branchless.glsl"

vec3 getNormalFromMap(vec3 world, vec2 tex, vec3 normal, sampler2D map)
{
	vec3 textureValue = texture(map, tex).xyz;
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