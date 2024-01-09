#SShader "Renderer/PostProcessing/Shader"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment
#version 460

#include "Stulu/StdLib.glsl"
#include "Stulu/Renderer/PostProcessing/PostProcessing.glsl"

in vec2 v_tex;

layout (binding = 0) uniform sampler2D texSampler;
layout (binding = 1) uniform sampler2D bloom;

out vec4 a_color;

void main() {
	vec4 color = texture2D(texSampler, v_tex);
	
	color = apply_bloom(color, bloomStrength, v_tex, bloom);

	a_color += gammaCorrect(color, gamma, toneMappingExposure) * when_eq(enableGammaCorrection, 1.0);
	a_color += color * when_neq(enableGammaCorrection, 1.0);

	if(color.a == 0.0)
		discard;
}