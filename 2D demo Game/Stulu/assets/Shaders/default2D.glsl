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
	vec4 _color = texture(u_textures[int(v_textureIndex)], v_texCoord * v_textureTiling) * v_color;
	if(_color.a < .01f)
		return;
	color = _color;
}