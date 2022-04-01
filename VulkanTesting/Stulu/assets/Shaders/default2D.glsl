##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_textureIndex;
layout(location = 4) in vec2 a_textureTiling;

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
struct VertData{
	vec2 texCoord;
	vec4 color;
	vec2 textureTiling;
};

layout (location = 0) out VertData Output;
layout (location = 3) out flat float textureIndex;

void main() {
	Output.texCoord = a_texCoord;
	Output.color = a_color;
	textureIndex = a_textureIndex;
	Output.textureTiling = a_textureTiling;

	gl_Position = viewProjection * vec4(a_pos, 1.0);
}



##type fragment
#version 460 core
layout(location = 0) out vec4 a_color;

struct VertData{
	vec2 texCoord;
	vec4 color;
	vec2 textureTiling;
};

layout (location = 0) in VertData Input;
layout (location = 3) in flat float textureIndex;

layout(binding = 2)uniform sampler2D u_textures[32];

void main() {
	//vec4 color = texture(u_textures[int(Input.textureIndex)], Input.texCoord * Input.textureTiling) * Input.color; //amd does not support this
	vec4 color = vec4(0.0f);
	switch(int(textureIndex))
	{
		case  0: color = texture(u_textures[ 0], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  1: color = texture(u_textures[ 1], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  2: color = texture(u_textures[ 2], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  3: color = texture(u_textures[ 3], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  4: color = texture(u_textures[ 4], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  5: color = texture(u_textures[ 5], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  6: color = texture(u_textures[ 6], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  7: color = texture(u_textures[ 7], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  8: color = texture(u_textures[ 8], Input.texCoord * Input.textureTiling) * Input.color; break;
		case  9: color = texture(u_textures[ 9], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 10: color = texture(u_textures[10], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 11: color = texture(u_textures[11], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 12: color = texture(u_textures[12], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 13: color = texture(u_textures[13], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 14: color = texture(u_textures[14], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 15: color = texture(u_textures[15], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 16: color = texture(u_textures[16], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 17: color = texture(u_textures[17], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 18: color = texture(u_textures[18], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 19: color = texture(u_textures[19], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 20: color = texture(u_textures[20], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 21: color = texture(u_textures[21], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 22: color = texture(u_textures[22], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 23: color = texture(u_textures[23], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 24: color = texture(u_textures[24], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 25: color = texture(u_textures[25], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 26: color = texture(u_textures[26], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 27: color = texture(u_textures[27], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 28: color = texture(u_textures[28], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 29: color = texture(u_textures[29], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 30: color = texture(u_textures[30], Input.texCoord * Input.textureTiling) * Input.color; break;
		case 31: color = texture(u_textures[31], Input.texCoord * Input.textureTiling) * Input.color; break;
	}
	a_color = color;
}