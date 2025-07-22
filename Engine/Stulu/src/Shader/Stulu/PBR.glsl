#ifndef _STULU_PRR_GLSL_
#define _STULU_PRR_GLSL_

#define ST_PBR

#include "Stulu/StdLib.glsl"
#include "Stulu/Renderer/Lighting.glsl"


struct PBRData {
	vec4  albedo;
	vec3  emission;
	float metallic;
	float roughness;
	float ao;

	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};

struct PBRResult { 
	vec3 color;
	float alpha;
	vec3 normal;
};

PBRResult ComputePBR(const PBRData data) {
	vec3 albedo = data.albedo.xyz;
	float alpha = data.albedo.w;

	vec3 normal = normalize(data.normal);
	vec3 view = normalize(cameraPosition.xyz - data.worldPos);
	vec3 F0 = mix(vec3(0.04), albedo, data.metallic);; 

	//lighting
	LightComputeData lightData;
	lightData.worldPos = data.worldPos;
	lightData.view = view;
	lightData.normal = normal;
	lightData.albedo = albedo;
	lightData.roughness = data.roughness;
	lightData.metallic = data.metallic;
	lightData.F0 = F0;

	vec3 lightingOut = data.emission;
	for(int i = 0; i < GetNumLights(); i++){

		vec3 lightOut = ComputeOutgoingLight(lights[i], lightData);

		float lightShadow = 0.0;
		if(IsLightMainCaster(i)) {
			lightShadow += ComputeCSMShadow(vec4(data.worldPos, 1.0), normal, lights[i]);
		}

		lightingOut += lightOut * saturate(1.0 - lightShadow);
	}

	vec3 F = fresnelSchlickRoughness(max(dot(normal, view), 0.0), F0, data.roughness);
	vec3 kD = (1.0 - F) * (1.0 - data.metallic);
  
	vec3 specular = vec3(0.0);
	vec3 diffuse = albedo * vec3(clearColor);

	if(useSkybox) {
		vec3 SB_N = GetSkyBoxCoords(normal, skyBoxRotation);
		vec3 SB_V = GetSkyBoxCoords(view, skyBoxRotation);
		vec3 SB_R = GetSkyBoxCoords(reflect(-view, normal), skyBoxRotation);
		
		vec3 irradiance = texture(irradianceMap, SB_N).rgb * environmentStrength;
		diffuse = irradiance * albedo;

		vec2 brdf  = texture(BRDFLUTMap, vec2(max(dot(SB_N, SB_V), 0.0), data.roughness)).rg;
		vec3 prefilteredColor = textureLod(prefilterMap, SB_R, data.roughness * MAX_REFLECTION_LOD).rgb * environmentReflections;
		specular = prefilteredColor * (F * brdf.x + brdf.y);
	}

	vec3 ambient = ((kD * diffuse * alpha) + specular) * data.ao;

	PBRResult res;
	res.normal = normal;
	res.alpha = alpha;
	res.color = (ambient + lightingOut);	

	if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayLighting))
		res.color = vec3(lightingOut);
	if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDepth))
		res.color = vec3(linearizeDepth(gl_FragCoord.z, cameraNearFar.x, cameraNearFar.y));
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDiffuse))
		res.color = vec3(diffuse);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplaySpecular))
		res.color = vec3(specular);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayAmbient))
		res.color = vec3(ambient);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayAmbientOcclusion))
		res.color = vec3(data.ao);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayNormal))
		res.color = saturate(vec3(normal)) * 0.5 + 0.5;
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayRoughness))
		res.color = vec3(data.roughness);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayMetallic))
		res.color = vec3(data.metallic);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayTexCoords))
		res.color = vec3(data.texCoords, 0);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayVertices))
		res.color = vec3(albedo);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayEmission))
		res.color = vec3(data.emission);

	return res;
}
#endif