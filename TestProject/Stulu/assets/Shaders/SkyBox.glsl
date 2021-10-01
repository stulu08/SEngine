##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
out vec3 v_texCoords;
void main(){
	v_texCoords = a_pos;
	mat4 view = mat4(mat3(viewMatrix));
	vec4 pos = projMatrix * view * vec4(a_pos, 1.0f);
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