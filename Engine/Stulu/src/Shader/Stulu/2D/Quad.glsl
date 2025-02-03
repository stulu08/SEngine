#SShader "2D/Quad"

#type vertex
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_textureIndex;
layout(location = 4) in vec2 a_textureTiling;

#include "Stulu/Scene.glsl"

layout (location = 0) out vec2 texCoord;
layout (location = 1) out vec4 color;
layout (location = 2) out vec2 textureTiling;
layout (location = 3) out flat float textureIndex;

void main() {
	texCoord = a_texCoord;
	color = a_color;
	textureIndex = a_textureIndex;
	textureTiling = a_textureTiling;

	gl_Position = viewProjection * vec4(a_pos, 1.0);
}
#type fragment
layout(location = 0) out vec4 a_color;

layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 textureTiling;
layout (location = 3) in flat float textureIndex;

layout (binding = 0) uniform sampler2D u_textures[32];

void main() {
	a_color = vec4(0.0);

	switch(int(textureIndex))
	{
		case  0: a_color = texture(u_textures[ 0], texCoord * textureTiling) * color; break;
		case  1: a_color = texture(u_textures[ 1], texCoord * textureTiling) * color; break;
		case  2: a_color = texture(u_textures[ 2], texCoord * textureTiling) * color; break;
		case  3: a_color = texture(u_textures[ 3], texCoord * textureTiling) * color; break;
		case  4: a_color = texture(u_textures[ 4], texCoord * textureTiling) * color; break;
		case  5: a_color = texture(u_textures[ 5], texCoord * textureTiling) * color; break;
		case  6: a_color = texture(u_textures[ 6], texCoord * textureTiling) * color; break;
		case  7: a_color = texture(u_textures[ 7], texCoord * textureTiling) * color; break;
		case  8: a_color = texture(u_textures[ 8], texCoord * textureTiling) * color; break;
		case  9: a_color = texture(u_textures[ 9], texCoord * textureTiling) * color; break;
		case 10: a_color = texture(u_textures[10], texCoord * textureTiling) * color; break;
		case 11: a_color = texture(u_textures[11], texCoord * textureTiling) * color; break;
		case 12: a_color = texture(u_textures[12], texCoord * textureTiling) * color; break;
		case 13: a_color = texture(u_textures[13], texCoord * textureTiling) * color; break;
		case 14: a_color = texture(u_textures[14], texCoord * textureTiling) * color; break;
		case 15: a_color = texture(u_textures[15], texCoord * textureTiling) * color; break;
		case 16: a_color = texture(u_textures[16], texCoord * textureTiling) * color; break;
		case 17: a_color = texture(u_textures[17], texCoord * textureTiling) * color; break;
		case 18: a_color = texture(u_textures[18], texCoord * textureTiling) * color; break;
		case 19: a_color = texture(u_textures[19], texCoord * textureTiling) * color; break;
		case 20: a_color = texture(u_textures[20], texCoord * textureTiling) * color; break;
		case 21: a_color = texture(u_textures[21], texCoord * textureTiling) * color; break;
		case 22: a_color = texture(u_textures[22], texCoord * textureTiling) * color; break;
		case 23: a_color = texture(u_textures[23], texCoord * textureTiling) * color; break;
		case 24: a_color = texture(u_textures[24], texCoord * textureTiling) * color; break;
		case 25: a_color = texture(u_textures[25], texCoord * textureTiling) * color; break;
		case 26: a_color = texture(u_textures[26], texCoord * textureTiling) * color; break;
		case 27: a_color = texture(u_textures[27], texCoord * textureTiling) * color; break;
		case 28: a_color = texture(u_textures[28], texCoord * textureTiling) * color; break;
		case 29: a_color = texture(u_textures[29], texCoord * textureTiling) * color; break;
		case 30: a_color = texture(u_textures[30], texCoord * textureTiling) * color; break;
		case 31: a_color = texture(u_textures[31], texCoord * textureTiling) * color; break;
	}
	if(a_color.a == 0.0)
		discard;
}