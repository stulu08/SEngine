#SShader "Renderer/PostProcessing/Bloom"
#include "Stulu/Renderer/PostProcessing/EffectShader.glsl"

layout(std140, binding = ST_BUFFER_POSTPROCESS_BIND) uniform postProcessing
{
	float strength;
};

layout (binding = 0) uniform sampler2D sourceTexture;
layout (binding = 1) uniform sampler2D bloomResult;

vec4 ApplyEffect(vec2 textureCoord) { 
	vec4 sourceColor = texture(sourceTexture, textureCoord);
	vec3 bloomColor = texture(bloomResult, textureCoord).rgb;

	vec3 result = (sourceColor.rgb + (bloomColor * 1.0));

	return vec4(result, sourceColor.a);
}