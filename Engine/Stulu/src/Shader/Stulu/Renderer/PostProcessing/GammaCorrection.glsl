#SShader "Renderer/PostProcessing/GammaCorrection"

#include "Stulu/Renderer/PostProcessing/EffectShader.glsl"
#include "Stulu/Renderer/PostProcessing/Aces.glsl"

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
	return vec3(1.0) - exp(-color);
}
vec3 ReinhardExtended(const vec3 color) {
	const vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

vec3 GetToneMappedColor(const vec3 color)
{
	const vec3 exposedColor = color.rgb * exposure;

	vec3 mappedColor = exposedColor.rgb * when_zero(toneMappingMode);

	mappedColor += Reinhard(exposedColor.rgb) * when_eq(toneMappingMode, 1.0);
	mappedColor += ReinhardExtended(exposedColor.rgb) * when_eq(toneMappingMode, 2.0);
	mappedColor += ACES_Fitted(exposedColor.rgb) * when_eq(toneMappingMode, 3.0);

	return mappedColor;
}

layout (binding = 0) uniform sampler2D sourceTexture;


vec4 ApplyEffect(vec2 textureCoord) { 
	vec4 sourceColor = texture(sourceTexture, textureCoord);

	vec3 mappedColor = GetToneMappedColor(sourceColor.rgb);

	vec3 result = mix(mappedColor, GammaCorrection(mappedColor), enableGammaCorrection);

	return vec4(result.rgb, sourceColor.a);
}