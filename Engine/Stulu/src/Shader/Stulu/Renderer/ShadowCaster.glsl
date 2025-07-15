#SShader "Renderer/ShadowCaster"

#type vertex
#include "Stulu/MeshLayout.glsl"
#include "Stulu/Buffer/DefaultModelBuffer.glsl"

void main(){
	gl_Position = GetTranslationMatrix() * vec4(a_pos, 1.0);
}

#type geometry
#define VERTEX_COUNT 3
#include "Stulu/Scene.glsl"

layout(triangles, invocations = ST_MAX_SHADOW_CASCADES) in;
layout(triangle_strip, max_vertices = VERTEX_COUNT) out;

void main() {
    if (gl_InvocationID >= cascadeCount)
        return;

	for (int i = 0; i < VERTEX_COUNT; ++i)
	{
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  

#type fragment

#include "Stulu/Renderer/Lighting.glsl"

void main(){

}