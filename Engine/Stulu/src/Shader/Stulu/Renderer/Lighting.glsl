#ifndef _STULU_LIGHTING_GLSL_
#define _STULU_LIGHTING_GLSL_

#include "Stulu/Math.glsl"
#include "Stulu/Branchless.glsl"
#include "Stulu/Buffer/LightBuffer.glsl"
#include "Stulu/Renderer/Shadows.glsl"

struct LightComputeData {
	vec3 worldPos;
	vec3 view;
	vec3 normal;
	vec3 albedo;
	float roughness;
	float metallic;
	vec3 F0;
};
vec3 ComputeOutgoingLight(const Light light, const LightComputeData data){
	// Unpack data
	vec3 lightColor   = light.ColorStrength.rgb;
	float strength    = light.ColorStrength.a;
	vec3 lightPos     = light.PositionType.xyz;
	float lightType   = light.PositionType.w;
	vec3 lightDir     = normalize(-light.Direction.xyz); // forward direction
	vec4 extra        = light.Data;

	vec3 L, H;
	float attenuation = 1.0;
	vec3 radiance;

	 // Directional Light
	if (lightType == LIGHT_TYPE_DIR) {
		L = lightDir;
		H = normalize(data.view + L);
		radiance = lightColor * strength;
	}

	// Point Light
	else if (lightType == LIGHT_TYPE_POINT) {
		vec3 delta = lightPos - data.worldPos;
		float dist = length(delta);
		L = normalize(delta);
		H = normalize(data.view + L);

		float radius = extra.x;

		attenuation = clamp(1.0 - dist / radius, 0.0, 1.0);
		radiance = lightColor * strength * attenuation;
	}
	// Spot Light
	else if (lightType == LIGHT_TYPE_SPOT) {
		L = normalize(lightPos - data.worldPos);
		H = normalize(data.view + L);

		float inner = extra.x;
		float outer = extra.y;

		float theta = dot(L, lightDir);
		float epsilon = max(inner - outer, 0.001);
		float spotIntensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

		radiance = lightColor * strength * theta;
	} 

	 // PBR Cook-Torrance BRDF
	float NDF = DistributionGGX(data.normal, H, data.roughness);        
	float G   = GeometrySmith(data.normal, data.view, L, data.roughness);      
	vec3 F    = fresnelSchlick(max(dot(H, data.view), 0.0), data.F0); 

	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - data.metallic);	 

	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(data.normal, data.view), 0.0) * max(dot(data.normal, L), 0.0);
	vec3 specular     = numerator / max(denominator, 0.001);  

	float NdotL = max(dot(data.normal, L), 0.0);                
	return (kD * data.albedo / PI + specular) * radiance * NdotL;
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