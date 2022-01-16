##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in float a_metallic;
layout (location = 4) in float a_roughness;
layout (location = 5) in float a_ao;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
    mat4 u_transform;
};
struct vertOutput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec3 cameraPosition;
	vec3 cameraRotation;
    float metallic;
    float roughness;
    float ao;
};
layout (location = 0) out vertOutput _output;
void main()
{
    _output.metallic = a_metallic;
    _output.roughness = a_roughness;
    _output.ao = a_ao;
    _output.worldPos = vec3(u_transform * vec4(a_pos, 1.0));
    _output.normal = mat3(transpose(inverse(u_transform))) * a_normal;
    _output.texCoords = a_texCoords;
    _output.cameraPosition = vec3(cameraPosition);
    _output.cameraRotation = vec3(cameraRotation);
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
##add ST_functions
out vec4 FragColor;
struct Light{
    vec4 colorAndStrength;
    vec4 positionAndType;   
    vec4 rotation;
    vec4 spotLightData;
};
layout(std140, binding = 1) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
layout(std140, binding = 2) uniform postProcessing
{
	float toneMappingExposure;
	float gamma;
};
struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec3 cameraPosition;
	vec3 cameraRotation;
    float metallic;
    float roughness;
    float ao;
};
layout (location = 0) in vertInput vertex;
layout (binding = 1) uniform samplerCube skybox;
layout(binding = 3)uniform sampler2D atlas;

struct Material{
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
};
Material u_mat;
void main()
{
    u_mat.albedo = vec3(texture(atlas, vertex.texCoords));
    u_mat.metallic = vertex.metallic;
    u_mat.roughness = vertex.roughness;
    u_mat.ao = vertex.ao;

    vec3 cameraPosition = vec3(vertex.cameraPosition);
    vec3 cameraRotation = vec3(vertex.cameraRotation);

    vec3 N = normalize(vertex.normal);
    vec3 V = normalize(cameraPosition - vertex.worldPos);
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
    // gamma correct
    //color = pow(color, vec3(1.0/gamma)); 

    FragColor = vec4(color , 1.0);

}