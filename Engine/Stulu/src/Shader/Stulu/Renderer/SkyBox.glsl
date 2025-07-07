#SShader "Renderer/SkyBox"
#Expose Skybox("Texture", auto_align)

#type vertex
#include "Stulu/MeshLayout.glsl"
#include "Stulu/Scene.glsl"

layout (location = 0) out vec3 v_texCoords;

void main() {
	v_texCoords = a_pos;

	mat4 view = mat4(mat3(viewMatrix));
	vec4 pos = projMatrix * view * vec4(a_pos, 1.0);

	gl_Position = pos.xyww;
}

#type fragment

#include "Stulu/Utils.glsl"
#include "Stulu/Renderer/Lighting.glsl"
#include "Stulu/Out.glsl"

layout (location = 0) in vec3 v_texCoords;

layout(std140, binding = ST_USER_MATERIAL_BINDING) uniform Material {
	float hasSkybox;
};

void main() {
	const vec3 view = getSkyBoxCoords(v_texCoords, skyBoxRotation);
	
	vec3 texColor = textureLod(environmentMap, view, env_lod).rgb;
	vec3 outColor = mix(clearColor.rgb, texColor, hasSkybox);

	vec3 worldPos = cameraPosition.xyz + normalize(v_texCoords) * 5000.0;
	ApplyHorizonFog(outColor, worldPos);
	
	WriteDefaultOut(outColor);
	
}