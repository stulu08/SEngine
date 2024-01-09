#SShader "Renderer/CubeMap/DefaultShader"

#type vertex
#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec4 a_color;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	WorldPos = a_pos;  
	gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#type fragment
#version 460 core

out vec4 FragColor;
in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;

void main()
{		
	FragColor = vec4(texture(environmentMap, WorldPos).rgb, 1.0);
}