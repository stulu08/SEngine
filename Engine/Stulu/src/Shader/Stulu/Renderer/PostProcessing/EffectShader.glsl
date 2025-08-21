#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment

#include "Stulu/StdLib.glsl"

layout (location = 0) in vec2 ST_TextureCoord;
layout (location = 0) out vec4 ST_OutColor;

vec4 ApplyEffect(vec2 textureCoord);

void main() {
	vec4 color = ApplyEffect(ST_TextureCoord);

	ST_OutColor = color;

	if(color.a == 0.0)
		discard;
}