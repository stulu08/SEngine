##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

void main()
{
    FragPos = vec3(u_transform * vec4(a_pos, 1.0));
    Normal = a_normal;  
    
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 u_lightPos; 
uniform vec3 u_lightColor;
uniform vec3 u_color;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;
            
    vec3 result = (ambient + diffuse) * u_color;
    color = vec4(result, 1.0);
} 