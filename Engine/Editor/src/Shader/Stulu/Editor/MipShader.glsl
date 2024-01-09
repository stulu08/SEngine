#SShader "Editor/MipShader"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment
#version 460

in vec2 v_tex;
out vec4 a_color;

layout (binding = 0) uniform sampler2D u_texture;
uniform float u_level = 0.0;


void main() {
	vec3 color = textureLod(u_texture, v_tex, u_level).rgb;
	a_color = vec4(color, 1.0);
}