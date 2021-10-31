##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
struct vertOutput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec3 cameraPosition;
};
layout (location = 0) out vertOutput _output;
uniform mat4 u_transform;


void main()
{
    _output.worldPos = vec3(u_transform * vec4(a_pos, 1.0));
    _output.normal = mat3(transpose(inverse(u_transform))) * a_normal;
    _output.texCoords = a_texCoords;
    _output.cameraPosition = cameraPosition;
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
out vec4 FragColor;

struct Light{
    vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
    vec4 spotLightData;
};

const int maxLights = 25;

layout(std140, binding = 1) uniform lightData
{
    Light lights[maxLights];
    uint lightCount;
};

struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec3 cameraPosition;
};

layout (location = 0) in vertInput _input;
layout(binding = 1) uniform samplerCube skybox;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;


// material parameters
struct Material{
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
};
Material u_mat = {vec3(.5,.5,.5),1.0,.5,.0};

  
const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
} 
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, _input.texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(_input.worldPos);
    vec3 Q2  = dFdy(_input.worldPos);
    vec2 st1 = dFdx(_input.texCoords);
    vec2 st2 = dFdy(_input.texCoords);

    vec3 N   = normalize(_input.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
void main()
{	

    u_mat.albedo = pow(texture(albedoMap, _input.texCoords).rgb, vec3(2.2));
    u_mat.metallic = texture(metallicMap, _input.texCoords).r;
    u_mat.roughness = texture(roughnessMap, _input.texCoords).r;
    u_mat.ao = texture(aoMap, _input.texCoords).r;
    
    //vec3 N = normalize(_input.normal);
    vec3 N = getNormalFromMap();
    vec3 V = normalize(_input.cameraPosition - _input.worldPos);
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
            vec3 L = normalize(lights[i].positionAndType.xyz - _input.worldPos);
            vec3 H = normalize(V + L);
            float distance    = length(lights[i].positionAndType.xyz - _input.worldPos);
            float radius = lights[i].spotLightData.w;
            float attenuation = max(1-(distance / radius),0);
            
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
            vec3 L = normalize(lights[i].positionAndType.xyz - _input.worldPos);
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
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);

}