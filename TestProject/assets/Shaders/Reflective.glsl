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
    Normal = a_normal;  
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;


// lights

uniform vec3 u_camPos;
uniform samplerCube skybox;
uniform float u_metallic;


void main()
{		
    vec3 view = normalize(WorldPos.xyz - u_camPos);
    vec3 refelcted = reflect(view, normalize(Normal));
    vec4 relectedColor = texture(skybox, refelcted);
   


    FragColor = relectedColor * u_metallic;
}  