##add ST_default
// material parameters
layout(std140, binding = 3) uniform material {
    vec4  albedo;
    float metallic;
    float roughness;
    float ao;
};
struct Material {
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
} u_mat;
void main()
{
    u_mat = Material(vec3(albedo), metallic, roughness, ao);
    vec3 cameraPos = vec3(cameraPosition);
    vec3 cameraRot = vec3(cameraRotation);
    vec3 N = normalize(vertex.normal);
    vec3 V = normalize(cameraPos - vertex.worldPos);
    vec3 R = reflect(-V, N);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, u_mat.albedo, u_mat.metallic);
	
    vec3 Lo = vec3(0.0);
    // reflectance equation
    for(int i = 0; i < lightCount; i++){
        //directional
        if(lights[i].positionAndType.w == 0){
            
             // calculate per-light radiance
            vec3 L = normalize(-lights[i].rotation.xyz);
            vec3 H = normalize(V + L);
            float attenuation = 1.0f;

            vec3 radiance     = lights[i].colorAndStrength.xyz * attenuation;        
    
            // cook-torrance brdf
            float NDF = DistributionGGX(N, H, u_mat.roughness);        
            float G   = GeometrySmith(N, V, L, u_mat.roughness);      
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - u_mat.metallic;	  
    
            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular     = numerator / denominator;  

            // add to outgoing radiance Lo
            float NdotL = max(dot(N, L), 0.0);                
            Lo += (kD * u_mat.albedo / PI + specular) * radiance * NdotL * lights[i].colorAndStrength.w;
            
        }
        //point
        else if(lights[i].positionAndType.w == 1){
            // calculate per-light radiance
            vec3 L = normalize(lights[i].positionAndType.xyz - vertex.worldPos);
            vec3 H = normalize(V + L);
            float distance = length(lights[i].positionAndType.xyz - vertex.worldPos);
            float radius = lights[i].spotLightData.w;
            //float attenuation = 1.0 / (distance * distance);
            float attenuation = 1.0f-(min(distance/radius,1.0f));
            
            vec3 radiance     = lights[i].colorAndStrength.xyz * attenuation;        
    
            // cook-torrance brdf
            float NDF = DistributionGGX(N, H, u_mat.roughness);        
            float G   = GeometrySmith(N, V, L, u_mat.roughness);      
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - u_mat.metallic;	  
    
            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
            vec3 specular     = numerator / max(denominator, 0.001);  
        
            // add to outgoing radiance Lo
            float NdotL = max(dot(N, L), 0.0);                
            Lo += (kD * u_mat.albedo / PI + specular) * radiance * NdotL * lights[i].colorAndStrength.w;
        }
        //spot
        else if(lights[i].positionAndType.w == 2) {
            vec3 L = normalize(lights[i].positionAndType.xyz - vertex.worldPos);
            float theta = dot(L, normalize(-lights[i].rotation.xyz));
            float epsilon   = lights[i].spotLightData.x - lights[i].spotLightData.y;
            float intensity = clamp((theta - lights[i].spotLightData.y) / epsilon, 0.0, 1.0);    

            float attenuation = 1.0f;

            vec3 radiance     = lights[i].colorAndStrength.xyz * attenuation;        

            vec3 H = normalize(V + L);
      
            // cook-torrance brdf
            float NDF = DistributionGGX(N, H, u_mat.roughness);        
            float G   = GeometrySmith(N, V, L, u_mat.roughness);      
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - u_mat.metallic;	  
    
            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
            vec3 specular     = numerator / max(denominator, 0.001);  
        
            // add to outgoing radiance Lo
            float NdotL = max(dot(N, vec3(intensity)), 0.0);                
            Lo += (kD * u_mat.albedo / PI + specular) * radiance * NdotL * lights[i].colorAndStrength.w;
        }
    }
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, u_mat.roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - u_mat.metallic;	  
    
    vec3 irradiance = vec3(1.0);
    vec3 diffuse      = irradiance * u_mat.albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(skybox, R, u_mat.roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = vec2(.75*u_mat.roughness);
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * u_mat.ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    //color = color / (color + vec3(toneMappingExposure));
    color = vec3(1.0f) - exp(-color * toneMappingExposure);
    // gamma correct
    color = pow(color, vec3(1.0/gamma)); 

    FragColor = vec4(color , 1.0);

}