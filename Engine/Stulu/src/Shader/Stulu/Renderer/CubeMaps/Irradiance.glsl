#SShader "Renderer/CubeMap/Irradiance"

#type vertex
#include "Stulu/MeshLayout.glsl"

layout (location = 0) out vec3 WorldPos;

layout (location = 0) uniform mat4 projection;
layout (location = 1) uniform mat4 view;

void main()
{
	WorldPos = a_pos;  
	gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#type fragment

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{		
	vec3 N = normalize(WorldPos);

	vec3 irradiance = vec3(0.0);   

	// tangent space calculation from origin point
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up         = normalize(cross(N, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0f;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}