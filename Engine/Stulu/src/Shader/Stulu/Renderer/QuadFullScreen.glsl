#SShader "Renderer/QuadFullScreen"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment

layout (location = 0) in vec2 v_tex;
layout (location = 0) out vec4 a_color;

layout (binding = 0) uniform sampler2D texSampler;

void main()
{
	vec4 color = texture(texSampler, v_tex);
	a_color = color;
	if(color.a == 0.0f)
		discard;
}