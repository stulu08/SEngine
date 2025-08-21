#SShader "Renderer/PostProcessing/GammaCorrection"

#include "Stulu/Renderer/PostProcessing/EffectShader.glsl"

layout(std140, binding = ST_BUFFER_POSTPROCESS_BIND) uniform postProcessing
{
	float enableGammaCorrection;
	float toneMappingMode;
	float exposure;
	float maxWhite;
	float gamma;
};

vec3 GammaCorrection(const vec3 color) {
	return pow(color, vec3(1.0 / gamma));
}

vec3 Reinhard(const vec3 color) {
	return vec3(1.0) - exp(-color.rgb * exposure);
}
vec3 ReinhardExtended(const vec3 color) {
	const vec3 exposedColor = color * exposure;
	const vec3 numerator = exposedColor * (1.0 + (exposedColor / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + exposedColor);
}
vec3 Aces(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

	// de gamma
	vec3 exposedColor = color * exposure;

    return saturate(
		(exposedColor * (a * exposedColor + b)) / (exposedColor * (c * exposedColor + d) + e)
	);
}

layout (binding = 0) uniform sampler2D sourceTexture;


vec4 ApplyEffect(vec2 textureCoord) { 
	vec4 sourceColor = texture(sourceTexture, textureCoord);

	vec3 mappedColor = sourceColor.rgb * when_zero(toneMappingMode);
	mappedColor += Reinhard(sourceColor.rgb) * when_eq(toneMappingMode, 1.0);
	mappedColor += ReinhardExtended(sourceColor.rgb) * when_eq(toneMappingMode, 2.0);
	mappedColor += Aces(sourceColor.rgb) * when_eq(toneMappingMode, 3.0);

	vec3 result = mix(mappedColor, GammaCorrection(mappedColor), enableGammaCorrection);

	return vec4(result.rgb, sourceColor.a);
}