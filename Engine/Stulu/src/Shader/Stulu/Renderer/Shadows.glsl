#ifndef _STULU_SHADOWS_GLSL_
#define _STULU_SHADOWS_GLSL_
#include "Stulu/Scene.glsl"
#include "Stulu/Poisson.glsl"

float GetCascadeFarPlane(int layer) {
	return cascadePlaneDistances[layer].x;
}
float GetCascadeNearPlane(int layer) {
	return cascadePlaneDistances[layer].y;
}
vec4 GetLightSpacePosition(const vec4 world, const int layer) {
	return lightSpaceMatrices[layer] * world;
}
float GetCascadeLightSize(int layer) {
	return cascadePlaneDistances[layer].z;
}

int SelectCSMLayer(float depthValue) {
	int layer = -1;
	for (int i = 0; i < int(cascadeCount); ++i) {
		if (depthValue < GetCascadeFarPlane(i)) {
			layer = i;
			break;
		}
	}
	if (layer == -1)
		layer = int(cascadeCount) - 1;
	
	return layer;
}

vec3 GetProjectedShadowCoords(const vec4 world, const int layer) { 
	const vec4 fragPosLightSpace = GetLightSpacePosition(world, layer);
	return vec3((fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5);
}

float CalculateBias(const vec3 pixelNormal, const vec3 lightDir) {
	return max(0.0005 * (1.0 - dot(pixelNormal, lightDir)), 0.0005);
}

float BlendCascades(float shadowA, float shadowB, float splitDist, float fragDepth) { 
	float blendStart = splitDist - CascadeBlendDistance;
	float blendEnd = splitDist;
	float blendFactor = smoothstep(blendStart, blendEnd, fragDepth);
	return mix(shadowA, shadowB, blendFactor);
}
// Basic hard shadows
float SampleHardShadow(const vec4 world, const int layer, const float bias) {
	vec3 projCoords = GetProjectedShadowCoords(world, layer);
	if (projCoords.z > 1.0)
		return 0.0;

	float closestDepth = texture(cascadeShadowMap, vec3(projCoords.xy, float(layer))).r;
	return (projCoords.z - bias > closestDepth) ? 0.0 : 1.0;
}

// Percantage Closer Filtering
float SampleCascadePCF(const vec4 world, const int layer, const float bias) {
	vec3 projCoords = GetProjectedShadowCoords(world, layer);
	if (projCoords.z > 1.0)
		return 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(cascadeShadowMap, 0));
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(cascadeShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, float(layer))).r;
			shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	return shadow / 9.0;
}

// Percantage Closer Filtering using Poisson Offset
float SamplePoisonPCF(const vec4 world, const int layer, const float bias, const float filterRadius) {
	vec3 projCoords = GetProjectedShadowCoords(world, layer);

	float shadow = 0.0;
	uint numSamples = GetPoissonSampleCount(ShadowQuality);

	for(int i = 0; i < numSamples; i++) {
		vec2 offset = GetPoissonValue(ShadowQuality, i) * filterRadius;
		float pcfDepth = texture(cascadeShadowMap, vec3(projCoords.xy + offset, float(layer))).r;
		shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;        
	}
	return shadow / float(numSamples);
}
float SamplePoisonPCF(const vec4 world, const int layer, const float bias) {
	float filterRadius = 1.0 / textureSize(cascadeShadowMap, 0).x;
	return SamplePoisonPCF(world, layer, bias, filterRadius);
}

// Percatage Close Soft Shadows 
vec2 FindOccluder(const vec4 world, const int layer, const float zRec, const float bias) {
	const vec3 projCoords = GetProjectedShadowCoords(world, layer);
	const float searchArea = GetCascadeLightSize(layer) * (zRec - GetCascadeNearPlane(layer)) / zRec;

	float occluder = 0.0;
	float count = 0.0;

	// use poisson samples here 
	const int poisonQuality = ShadowQuality;
	const int numSamples = GetPoissonSampleCount(poisonQuality);

	for(int i = 0; i < numSamples; i++) {
		vec2 offset = GetPoissonValue(poisonQuality, i) * searchArea;
		float depth = texture(cascadeShadowMap, vec3(projCoords.xy + offset, float(layer))).r;
		if((projCoords.z - bias) > depth) {
			occluder += depth;
			count += 1.0;
		}
	}

	return vec2(occluder / max(count, 1.0), count);
}

float SamplePCSS(const vec4 world, const int layer, const float bias) {
	const float zReceiver = GetLightSpacePosition(world, layer).w;
	
	vec2 occ = FindOccluder(world, layer, zReceiver, bias);
	// no occluders
    if (occ.y == 0.0) return 0.0;

    float avgBlocker = occ.x;
    float penumbra = GetCascadeLightSize(0) * (zReceiver - avgBlocker) / avgBlocker;
	float filterRadius = GetCascadeNearPlane(0) * penumbra / zReceiver;

	return SamplePoisonPCF(world, layer, bias, filterRadius);
}


float ComputeCSMShadow(const vec4 world, const vec3 pixelNormal, const vec3 lightDir) { 
	const float fragDepth = abs((viewMatrix * world).z);
	const int layer = SelectCSMLayer(fragDepth);
	const float bias = CalculateBias(pixelNormal, lightDir);

	// outside light view
	float shadow = SamplePCSS(world, layer, bias);

	// no blending for last layer
	if(layer == int(cascadeCount) - 1)
		return shadow;

	// blend with next layer
	float nextShadow = SamplePCSS(world, layer + 1, bias);
	return BlendCascades(shadow, nextShadow, GetCascadeFarPlane(layer), fragDepth);
}

#endif