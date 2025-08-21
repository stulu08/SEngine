#ifndef _STULU_COLOR_GLSL_
#define _STULU_COLOR_GLSL_

float linearizeDepth(float depth,float zNear,float zFar) {
    return zFar * zNear / (zFar - depth * (zFar - zNear));
}

vec3 srgbToLinear(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec4 srgbToLinear(vec4 color){
	return vec4(srgbToLinear(color.xyz), color.a);
}

vec4 saturate(vec4 col) {
    return clamp(col, vec4(0.0), vec4(1.0));
}
vec3 saturate(vec3 col) {
    return clamp(col, vec3(0.0), vec3(1.0));
}
float saturate(float col) {
    return clamp(col, float(0.0), float(1.0));
}



#endif