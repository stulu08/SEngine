#SShader "Renderer/CubeMap/EquirectangularToCubemap"

#type vertex
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec4 a_color;

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

layout (binding = 0) uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{		
	vec2 uv = SampleSphericalMap(normalize(WorldPos));
	vec3 color = texture(equirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}