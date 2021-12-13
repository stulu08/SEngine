##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

out vec3 Normal;
out vec3 WorldPos;
out vec3 CamPos;

layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
uniform mat4 u_transform;

void main()
{
    WorldPos = vec3(u_transform * vec4(a_pos, 1.0));
    Normal = mat3(transpose(inverse(u_transform))) * a_normal;
    CamPos = cameraPosition;
    gl_Position =  u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;
in vec3 CamPos;

layout(binding = 1) uniform samplerCube skybox;

void main()
{		
    vec3 view = normalize(WorldPos.xyz - CamPos);
    vec3 refelcted = reflect(view, normalize(Normal));
    vec4 relectedColor = texture(skybox, refelcted);
 

    FragColor = relectedColor;
}  