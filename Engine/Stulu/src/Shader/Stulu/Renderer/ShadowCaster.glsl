#SShader "Renderer/ShadowCaster"

#type vertex

#include "Stulu/MeshLayout.glsl"
#include "Stulu/Scene.glsl"

layout(std140, binding = 1) uniform modelData
{
	mat4 normalMatrix;
	mat4 transform;
};		
void main(){
	gl_Position = lightSpaceMatrix * transform * vec4(a_pos, 1.0);
}

#type fragment

#include "Stulu/Renderer/Lighting.glsl"

layout(location = 0) out float fragmentdepth;

void main(){
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	gl_FragDepth = gl_FragCoord.z;
	gl_FragDepth += gl_FrontFacing ? ST_BIAS : 0.0; 

	fragmentdepth = gl_FragCoord.z;
}