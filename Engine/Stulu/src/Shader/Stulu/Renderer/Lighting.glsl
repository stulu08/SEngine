#ifndef _STULU_LIGHTING_GLSL_
#define _STULU_LIGHTING_GLSL_

#include "Stulu/Internals.glsl"
#include "Stulu/Scene.glsl"
#include "Stulu/Math.glsl"
#include "Stulu/Branchless.glsl"
#include "Stulu/Buffer/LightBuffer.glsl"

#define ST_BIAS 0.01
float ComputeShadow(const vec4 fragPosLightSpace, sampler2D map, const vec3 normal, const vec3 lightDir) {
    // transform to [0,1] range
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(map, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // calculate bias
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.002);
	float bias = 0.002;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return 1.0 - shadow;
}
struct LightComputeData {
	vec3 worldPos;
	vec3 view;
	vec3 normal;
	vec3 albedo;
	float roughness;
	float metallic;
	vec3 F0;
};
vec3 ComputeOutgoingLight(const Light light, const LightComputeData data, inout vec3 lightDirection){
    float distance = length(light.positionAndType.xyz - data.worldPos);

	vec3 L;
	vec3 H;
	float attenuation = 1.0f;
	vec3 radiance;

	//calculate by light type
	if(light.positionAndType.w == 0){
		// calculate per-light radiance
		L = normalize(-light.rotation.xyz);
		H = normalize(data.view + L);
		radiance = light.colorAndStrength.xyz * attenuation;        
	}
	//point
	else if(light.positionAndType.w == 1){
		// calculate per-light radiance
		L = normalize(light.positionAndType.xyz - data.worldPos);
		H = normalize(data.view + L);
		float radius = light.spotLightData.w;
		attenuation = 1.0-(min(distance/radius,1.0));
		radiance     = light.colorAndStrength.xyz * attenuation;        
	}
	//spot
	else if(light.positionAndType.w == 2) {
		// calculate per-light radiance
		L = normalize(light.positionAndType.xyz - data.worldPos);
		H = normalize(data.view + L);
		float theta = dot(L, normalize(-light.rotation.xyz));
		float epsilon   = light.spotLightData.x - light.spotLightData.y;
		float intensity = clamp((theta - light.spotLightData.y) / epsilon, 0.0, 1.0);    
		attenuation = intensity;
		radiance     = light.colorAndStrength.xyz * attenuation;   
	}
	lightDirection = L;

	// cook-torrance brdf
	float NDF = DistributionGGX(data.normal, H, data.roughness);        
	float G   = GeometrySmith(data.normal, data.view, L, data.roughness);      
	vec3 F    = fresnelSchlick(max(dot(H, data.view), 0.0), data.F0); 
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - data.metallic;	 

	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(data.normal, data.view), 0.0) * max(dot(data.normal, L), 0.0);
	vec3 specular     = numerator / max(denominator, 0.001);  

	// add to outgoing radiance Lo
	float NdotL = max(dot(data.normal, L), 0.0);                
	return (kD * data.albedo / PI + specular) * radiance * NdotL * light.colorAndStrength.w;
}


#define FOG_AMMOUNT_CLAMP(expr) 1.0 - clamp((expr), 0.0, 1.0)


// f = (end - distance) / (end - start)
float FogAmountLinear(float fogDistance, float start, float end){
	return FOG_AMMOUNT_CLAMP((end - fogDistance) / (end - start));
}
// f = 1 / (e^(distance * density)) = 2^(-(log_2(e)) * distance * density)
float FogAmountExp(float fogDistance, float dens){
	float d = dens * fogDistance;
	return FOG_AMMOUNT_CLAMP(exp2(-LOG_2E * d));
}
// f = 1 / (e^(distance * density)^2) = 2^(-(log_2(e)) * (distance * density)^2)
float FogAmountExp2(float fogDistance, float dens){
	float d = dens * fogDistance;
	return FOG_AMMOUNT_CLAMP(exp2(-LOG_2E * d));
}

float GetFogStrength(vec3 vertexPosition) { 
	float fogDistance = distance(vertexPosition.xyz, cameraPosition.xyz);
	float fogAmount = 0.0;
	fogAmount += FogAmountLinear(fogDistance, linearFogStart, linearFogEnd) * when_eq(fogMode, 1.0);
	fogAmount += FogAmountExp(fogDistance, fogDensity) * when_eq(fogMode, 2.0);
	fogAmount += FogAmountExp2(fogDistance, fogDensity) * when_eq(fogMode, 3.0);
	return fogAmount;
} 

void ApplyDefaultFog(inout vec3 color, vec3 vertexPosition) {
	color = mix(color, fogColor.xyz, GetFogStrength(vertexPosition));
}

void ApplyHorizonFog(inout vec3 color, vec3 vertexPosition) {
    vec3 viewDir = normalize(vertexPosition - cameraPosition.xyz);

    // Horizon factor: 0 when looking up/down, 1 near horizon
    float horizon = abs(dot(viewDir, vec3(0.0, 1.0, 0.0)));
    float horizonFactor = pow(1.0 - horizon, FogHorizonFalloff);
    horizonFactor = smoothstep(0.0, 1.0, horizonFactor); // Optional softening

    // Height fog offset
    float verticalOffset = FogHorizonOffset;
    float heightFactor = clamp(1.0 - ((vertexPosition.y - verticalOffset) / FogHorizonHeightFalloff), 0.0, 1.0);
    heightFactor = smoothstep(0.0, 1.0, heightFactor);

    float horizonFogFactor = mix(horizonFactor, 1.0, FogHorizonStrength) * EnableFogHorizon;
    float heightFogFactor = heightFactor * EnableHorizonHeightFog;

	// Ground fog
    float groundFog = mix(0.0, clamp(1.0 - (vertexPosition.y / 30.0), 0.0, 1.0), FogGroundStrength) * EnableGroundFog;
    groundFog = smoothstep(0.0, 1.0, groundFog);

    float fogMultiplier = clamp(horizonFogFactor + heightFogFactor + groundFog, 0.0, 1.0);

    float fogAmount = GetFogStrength(vertexPosition) * fogMultiplier;

    color = mix(color, fogColor.rgb, fogAmount);
}

#endif