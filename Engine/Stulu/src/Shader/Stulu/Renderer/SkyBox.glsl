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
	vec3 view = getSkyBoxCoords(vertex.texCoords, skyBoxRotation);

	vec4 color = vec4(.0f);

	if(useSkybox){
		vec3 mapColor = clearColor.xyz;

		if(skyboxMapType == 0){
			//acces mip maps if needed
			if(env_lod == 0)
				mapColor = texture(environmentMap, view).rgb;
			else {
				mapColor = textureLod(environmentMap, view, env_lod).rgb;
			}
		} 
		else if(skyboxMapType == 1) {
			mapColor = texture(irradianceMap, view).rgb;
		} 
		else if(skyboxMapType == 2) {
			mapColor = texture(prefilterMap, view).rgb;
		}
		color = vec4(mapColor, 1.0f);
	}
	
	FragColor = color;
}