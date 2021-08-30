##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

out vec3 Normal;
out vec3 WorldPos;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

void main()
{
    WorldPos = vec3(u_transform * vec4(a_pos, 1.0));
    //Normal = a_normal;  
    Normal = mat3(transpose(inverse(u_transform))) * a_normal;
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
out vec4 FragColor;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform vec3  u_albedo = vec3(1.0f);
uniform float u_metallic = .5f;
uniform float u_roughness = .5f;
uniform float u_ao = .2f;

// lights
uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform float u_lightStrength;


uniform vec3 u_camPos;

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

void main()
{		
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_camPos - WorldPos);


    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, u_albedo, u_metallic);
	

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    vec3 L = normalize(u_lightPosition - WorldPos);
    vec3 H = normalize(V + L);
    float distance    = length(u_lightPosition - WorldPos);
    float attenuation = u_lightStrength / (distance * distance);

    vec3 radiance     = u_lightColor * attenuation;        
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, u_roughness);        
    float G   = GeometrySmith(N, V, L, u_roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * u_albedo / PI + specular) * radiance * NdotL; 
    
  
    vec3 ambient = vec3(0.03) * u_albedo * u_ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   


    FragColor = vec4(color, 1.0);
}  