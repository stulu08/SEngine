#SShader "Editor/MipShader"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment
layout (location = 0) in vec2 v_tex;
layout (location = 0) out vec4 a_color;

layout (binding = 0) uniform sampler2D u_texture;

layout(std140, binding = 1) uniform model {
	float z;
};

void main() {
	vec3 color = textureLod(u_texture, v_tex, z).rgb;
	a_color = vec4(color, 1.0);
}