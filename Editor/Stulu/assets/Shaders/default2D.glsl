##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_textureIndex;
layout(location = 4) in vec2 a_textureTiling;

uniform mat4 u_viewProjection;

out vec2 v_texCoord;
out vec4 v_color;
out float v_textureIndex;
out vec2 v_textureTiling;

void main() {
	v_texCoord = a_texCoord;
	v_color = a_color;
	v_textureIndex = a_textureIndex;
	v_textureTiling = a_textureTiling;
	gl_Position = u_viewProjection * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec2 v_texCoord;
in vec4 v_color;
in float v_textureIndex;
in vec2 v_textureTiling;

uniform sampler2D u_textures[32];

void main() {
	//vec4 _color = texture(u_textures[int(v_textureIndex)], v_texCoord * v_textureTiling) * v_color; amd does not support
	vec4 _color = v_color;
	switch(int(v_textureIndex))
	{
		case  0: _color *= texture(u_textures[ 0], v_texCoord * v_textureTiling); break;
		case  1: _color *= texture(u_textures[ 1], v_texCoord * v_textureTiling); break;
		case  2: _color *= texture(u_textures[ 2], v_texCoord * v_textureTiling); break;
		case  3: _color *= texture(u_textures[ 3], v_texCoord * v_textureTiling); break;
		case  4: _color *= texture(u_textures[ 4], v_texCoord * v_textureTiling); break;
		case  5: _color *= texture(u_textures[ 5], v_texCoord * v_textureTiling); break;
		case  6: _color *= texture(u_textures[ 6], v_texCoord * v_textureTiling); break;
		case  7: _color *= texture(u_textures[ 7], v_texCoord * v_textureTiling); break;
		case  8: _color *= texture(u_textures[ 8], v_texCoord * v_textureTiling); break;
		case  9: _color *= texture(u_textures[ 9], v_texCoord * v_textureTiling); break;
		case 10: _color *= texture(u_textures[10], v_texCoord * v_textureTiling); break;
		case 11: _color *= texture(u_textures[11], v_texCoord * v_textureTiling); break;
		case 12: _color *= texture(u_textures[12], v_texCoord * v_textureTiling); break;
		case 13: _color *= texture(u_textures[13], v_texCoord * v_textureTiling); break;
		case 14: _color *= texture(u_textures[14], v_texCoord * v_textureTiling); break;
		case 15: _color *= texture(u_textures[15], v_texCoord * v_textureTiling); break;
		case 16: _color *= texture(u_textures[16], v_texCoord * v_textureTiling); break;
		case 17: _color *= texture(u_textures[17], v_texCoord * v_textureTiling); break;
		case 18: _color *= texture(u_textures[18], v_texCoord * v_textureTiling); break;
		case 19: _color *= texture(u_textures[19], v_texCoord * v_textureTiling); break;
		case 20: _color *= texture(u_textures[20], v_texCoord * v_textureTiling); break;
		case 21: _color *= texture(u_textures[21], v_texCoord * v_textureTiling); break;
		case 22: _color *= texture(u_textures[22], v_texCoord * v_textureTiling); break;
		case 23: _color *= texture(u_textures[23], v_texCoord * v_textureTiling); break;
		case 24: _color *= texture(u_textures[24], v_texCoord * v_textureTiling); break;
		case 25: _color *= texture(u_textures[25], v_texCoord * v_textureTiling); break;
		case 26: _color *= texture(u_textures[26], v_texCoord * v_textureTiling); break;
		case 27: _color *= texture(u_textures[27], v_texCoord * v_textureTiling); break;
		case 28: _color *= texture(u_textures[28], v_texCoord * v_textureTiling); break;
		case 29: _color *= texture(u_textures[29], v_texCoord * v_textureTiling); break;
		case 30: _color *= texture(u_textures[30], v_texCoord * v_textureTiling); break;
		case 31: _color *= texture(u_textures[31], v_texCoord * v_textureTiling); break;
	}
	color = _color;
}