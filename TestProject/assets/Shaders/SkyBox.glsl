##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
uniform mat4 u_view;
uniform mat4 u_projection;
out vec3 v_texCoords;
void main(){
	v_texCoords = a_pos;
	vec4 pos = u_projection * u_view  * vec4(a_pos, 1.0f);
	gl_Position = pos.xyww;
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;
in vec3 v_texCoords;
uniform samplerCube u_skyBox;
void main() {
	color = texture(u_skyBox, v_texCoords);
}