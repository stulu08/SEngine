#SShader "Renderer/PostProcessing/Shader"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment

#include "Stulu/StdLib.glsl"
#include "Stulu/Renderer/PostProcessing/PostProcessing.glsl"

layout (location = 0) in vec2 v_tex;

layout (binding = 0) uniform sampler2D texSampler;
layout (binding = 1) uniform sampler2D bloom;

layout (location = 0) out vec4 a_color;

void main() {
	vec4 sourceColor = texture(texSampler, v_tex);
	
	// gamma correction
	vec4 outColor = gammaCorrect(sourceColor, gamma, toneMappingExposure);
	outColor += sourceColor * when_neq(enableGammaCorrection, 1.0);

	// applay bloom
	a_color = apply_bloom(outColor, bloomStrength, v_tex, bloom);
	
	if(a_color.a == 0.0)
		discard;
}