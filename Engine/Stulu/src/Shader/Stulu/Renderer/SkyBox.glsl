#SShader "Renderer/SkyBox"

#type vertex
		
#include "Stulu/MeshLayout.glsl"
#include "Stulu/Scene.glsl"

struct vertOutput
{
	vec3 texCoords;
};
layout (location = 0) out vertOutput v_output;

void main() {

	v_output.texCoords = a_pos;

	mat4 view = mat4(mat3(viewMatrix));
	vec4 pos = projMatrix * view * vec4(a_pos, 1.0);

	gl_Position = pos.xyww;
}

#type fragment

layout (location = 0) out vec4 FragColor;

#include "Stulu/Utils.glsl"
#include "Stulu/Scene.glsl"

struct vertInput
{
	vec3 texCoords;
};
layout (location = 0) in vertInput vertex;

void main() {
	vec4 color = vec4(clearColor.xyz, 1.0);
	if(useSkybox){
		vec3 view = getSkyBoxCoords(vertex.texCoords, skyBoxRotation);
		if(env_lod == 0)
			color = vec4(texture(environmentMap, view).rgb, 1.0);
		else {
			color = vec4(textureLod(environmentMap, view, env_lod).rgb, 1.0);
		}
	}
	
	FragColor = color;
}