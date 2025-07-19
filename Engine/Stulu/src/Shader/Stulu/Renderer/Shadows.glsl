#ifndef _STULU_SHADOWS_GLSL_
#define _STULU_SHADOWS_GLSL_
#include "Stulu/Scene.glsl"
#include "Stulu/Poisson.glsl"
#include "Stulu/Color.glsl"

float CalculateBias(const vec3 pixelNormal, const vec3 lightDir, const float biasMod /*= 0.0005*/) {
	return max(biasMod * (1.0 - dot(pixelNormal, lightDir)), 0.0005);
}

float GetCascadeFarPlane(int layer) {
	return cascadePlaneDistances[layer].x;
}
float GetCascadeNearPlane() {
	return cascadePlaneDistances[0].y;
}
float GetCascadeNearPlane(int layer) {
	return cascadePlaneDistances[layer].y;
}
vec3 GetProjectedCascedeShadowCoords(const vec4 world, const int layer) { 
	const vec4 fragPosLightSpace = lightSpaceMatrices[layer] * world;
	return vec3((fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5);
}

float ZClipToEye_Cascade(float z, const int layer) {
	return linearizeDepth(z, GetCascadeNearPlane(layer), GetCascadeFarPlane(layer));
}

int SelectCSMLayer(float depthValue) {
	int layer = -1;
	for (int i = 0; i < CascadeCount; ++i) {
		if (depthValue < GetCascadeFarPlane(i)) {
			layer = i;
			break;
		}
	}
	if (layer == -1)
		layer = CascadeCount - 1;
	
	return layer;
}

float BlendCascades(float shadowA, float shadowB, float splitDist, float fragDepth) { 
	float blendStart = splitDist - CascadeBlendDistance;
	float blendEnd = splitDist;
	float blendFactor = smoothstep(blendStart, blendEnd, fragDepth);
	return mix(shadowA, shadowB, blendFactor);
}
// Basic hard shadows
float SampleCascadeHardShadow(const vec3 shadowCoords, const int layer, const float bias) {
	float closestDepth = texture(CascadedShadowMap, vec3(shadowCoords.xy, float(layer))).r;
	return (shadowCoords.z - bias) > closestDepth ? 1.0 : 0.0;
}


// Percantage Closer Filtering using Poisson Offset
float SampleCascadePoisonPCF(const vec3 shadowCoords, const int layer, const float bias, const float filterRadius) {
	float shadow = 0.0;
	uint numSamples = GetPoissonSampleCount(ShadowQuality);

	for(int i = 0; i < numSamples; i++) {
		vec2 offset = GetPoissonValue(ShadowQuality, i) * filterRadius;
		float pcfDepth = texture(CascadedShadowMap, vec3(shadowCoords.xy + offset, float(layer))).r;
		shadow += (shadowCoords.z - bias) > pcfDepth ? 1.0 : 0.0;        
	}
	return shadow / float(numSamples);
}
// Percatage Close Soft Shadows 
vec2 FindCascadeOccluder(const vec3 shadowCoords, const int layer, const float bias) {
	const float searchArea = CascadeLightSize * (shadowCoords.z - GetCascadeNearPlane()) / cameraPosition.z;

	float occluder = 0.0;
	float count = 0.0;

	// use poisson samples here 
	const int numSamples = GetPoissonSampleCount(PCSSQuality);
	
	for(int i = 0; i < numSamples; i++) {
		vec2 offset = GetPoissonValue(PCSSQuality, i) * searchArea;
		float depth = texture(CascadedShadowMap, vec3(shadowCoords.xy + offset, float(layer))).r;
		if(shadowCoords.z > depth) {
			occluder += depth;
			count += 1.0;
		}
	}

	return vec2(occluder / max(count, 1.0), count);
}

float SampleCascadePCSS(const vec3 shadowCoords, const int layer, const float bias) {
	float zReceiver = shadowCoords.z;

	vec2 occ = FindCascadeOccluder(shadowCoords, layer, bias);
    if (occ.y == 0.0) return 0.0;

    float avgBlocker = linearizeDepth(occ.x, GetCascadeNearPlane(), GetCascadeNearPlane(CascadeCount));
    float penumbra = (zReceiver - avgBlocker) / avgBlocker;

	float filterRadius = CascadeLightSize * GetCascadeNearPlane() * penumbra / zReceiver;

	return SampleCascadePoisonPCF(shadowCoords, layer, bias, filterRadius);
}

float SampleCascadeMap(const vec4 world, const int layer, const float bias) { 
	vec3 shadowCoords = GetProjectedCascedeShadowCoords(world, layer);
	if (shadowCoords.z > 1.0)
		return 0.0;

	float shadow = 0.0;
	if(CascadeSoftShadows == 1.0)
		shadow += SampleCascadePCSS(shadowCoords, layer, bias);
	else
		shadow += SampleCascadeHardShadow(shadowCoords, layer, bias);

	return shadow;
}

float ComputeCSMShadow(const vec4 world, const vec3 pixelNormal, const Light light) { 
	const float fragDepth = abs((viewMatrix * world).z);
	const int layer = SelectCSMLayer(fragDepth);
	const float bias = CalculateBias(pixelNormal, normalize(-light.Direction.xyz), CascadeBiasMod);

	// outside light view
	float shadow = SampleCascadeMap(world, layer, bias);

	// no blending for last layer
	if(layer == CascadeCount - 1)
		return shadow;

	// blend with next layer
	float nextShadow = SampleCascadeMap(world, layer + 1, bias);
	return BlendCascades(shadow, nextShadow, GetCascadeFarPlane(layer), fragDepth);
}

#endif