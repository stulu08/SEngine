#SShader "Renderer/CubeMap/DefaultShader"

#type vertex

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec4 a_color;

layout (location = 0) out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	WorldPos = a_pos;  
	gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#type fragment

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;

void main()
{		
	FragColor = vec4(texture(environmentMap, WorldPos).rgb, 1.0);
}