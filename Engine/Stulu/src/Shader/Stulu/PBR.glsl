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
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	vec3 lightOut;
	vec3 normal;
	vec3 color;
	float alpha;
};

PBRResult ComputePBR(const PBRData data) {
	vec3 N = normalize(data.normal);
	vec3 V = normalize(cameraPosition.xyz - data.worldPos);
	vec3 R = reflect(-V, N);
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, data.albedo.rgb, data.metallic);

	//lighting
	LightComputeData lightData;
	lightData.worldPos = data.worldPos;
	lightData.view = V;
	lightData.normal = N;
	lightData.albedo = data.albedo.rgb;
	lightData.roughness = data.roughness;
	lightData.metallic = data.metallic;
	lightData.F0 = F0;

	vec3 lightingOut = data.emission;
	for(int i = 0; i < GetNumLights(); i++){

		vec3 lightOut = ComputeOutgoingLight(lights[i], lightData);

		float lightShadow = 0.0;
		if(IsLightMainCaster(i)) {
			lightShadow += ComputeCSMShadow(vec4(data.worldPos, 1.0), N, lights[i]);
		}

		lightingOut += lightOut * saturate(1.0 - lightShadow);
	}

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, data.roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - data.metallic;
  
	vec3 prefilteredColor = vec3(0.0);
	vec3 irradiance = vec3(clearColor);
	vec2 brdf = vec2(0.0);

	if(useSkybox) {
		vec3 SB_N = GetSkyBoxCoords(N, skyBoxRotation);
		vec3 SB_R = GetSkyBoxCoords(R, skyBoxRotation);
		vec3 SB_V = GetSkyBoxCoords(V, skyBoxRotation);
		
		irradiance = texture(irradianceMap, SB_N).rgb * environmentStrength;
		prefilteredColor = textureLod(prefilterMap, SB_R, data.roughness * MAX_REFLECTION_LOD).rgb * environmentReflections;
		brdf  = texture(BRDFLUTMap, vec2(max(dot(SB_N, SB_V), 0.0), data.roughness)).rg;
	}

	PBRResult res;
	res.lightOut = lightingOut;
	res.diffuse  = irradiance * data.albedo.rgb;
	res.specular = prefilteredColor * (F * brdf.x + brdf.y);
	res.ambient = ((kD * res.diffuse * data.albedo.a) + res.specular)*data.ao;
	res.normal = data.normal;
	res.color = (res.ambient + res.lightOut) * when_eq(viewFlags, 0);	
	res.alpha = data.albedo.a;

	//if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayLighting))
	//	res.color = vec3(res.lightOut);
	//if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDepth))
	//	res.color = vec3(linearizeDepth(gl_FragCoord.z, cameraNearFar.x, cameraNearFar.y));
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDiffuse))
	//	res.color = vec3(res.diffuse);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplaySpecular))
	//	res.color = vec3(res.specular);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayAmbient))
	//	res.color = vec3(res.ambient);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayAmbientOcclusion))
	//	res.color = vec3(data.ao);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayNormal))
	//	res.color = vec3(data.normal) * 0.5 + 0.5;
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayRoughness))
	//	res.color = vec3(data.roughness);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayMetallic))
	//	res.color = vec3(data.metallic);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayTexCoords))
	//	res.color = vec3(data.texCoords, 0);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayVertices))
	//	res.color = vec3(data.albedo.rgb);
	//else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayEmission))
	//	res.color = vec3(data.emission);

	return res;
}
#endif