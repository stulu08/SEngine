##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
uniform mat4 u_viewProjection;
out vec3 v_texCoords;
void main(){
	v_texCoords = a_pos;
	gl_Position = u_viewProjection * vec4(a_pos, 1.0f);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;
in vec3 v_texCoords;
uniform samplerCube skybox;
void main() {
	color = texture(skybox, v_texCoords);
}