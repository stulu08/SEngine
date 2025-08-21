#ifndef _STULU_TEXTURE_GLSL_
#define _STULU_TEXTURE_GLSL_

#include "Stulu/Color.glsl"

vec3 SampleNormalMap(vec3 worldPos, vec2 texCoord, vec3 normal, sampler2D normalMap, float useNormalMap) {
    vec3 tangentNormal = texture(normalMap, texCoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(worldPos);
    vec3 Q2 = dFdy(worldPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N = normalize(normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);
    vec3 mapped = normalize(TBN * tangentNormal);

    return mix(normal, mapped, useNormalMap);
}

float SampleTexture(sampler2D map, vec2 uv, float useMap, float fallback) {
    return mix(fallback, texture(map, uv).r, useMap);
}
vec3 SampleTexture(sampler2D map, vec2 uv, float useMap, vec3 fallback) {
    return mix(fallback,  texture(map, uv).rgb, useMap);
}
vec4 SampleTexture(sampler2D map, vec2 uv, float useMap, vec4 fallback) {
    return mix(fallback, texture(map, uv).rgba, useMap);
}
vec3 SampleColorTexture(sampler2D map, vec2 uv, float useMap, vec3 fallback) {
    vec3 tex = srgbToLinear(texture(map, uv).rgb);
    return mix(fallback, tex, useMap);
}
vec4 SampleColorTexture(sampler2D map, vec2 uv, float useMap, vec4 fallback) {
    vec4 texVal = texture(map, uv);
    return mix(fallback, texVal, useMap);
}

vec3 GetSkyBoxCoords(vec3 view, mat4 _skyBoxRotation) {
	return normalize(_skyBoxRotation * vec4(view, 0.0)).xyz;
}
vec3 SampleSkybox(samplerCube map, vec3 view, float useMap, vec3 fallback) {
    return mix(fallback, texture(map, view).rgb, useMap);
}
vec3 SampleSkyboxLod(samplerCube map, vec3 view, float useMap, vec3 fallback, float lod) {
    return mix(fallback, textureLod(map, view, lod).rgb, useMap);
}


#endif