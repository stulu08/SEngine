##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout(std140, binding = 0) uniform data
{
	mat4 u_viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 cameraPosition;
	vec3 cameraRotation;
};
uniform mat4 u_transform;
uniform int u_preview = 0;
out vec3 v_texCoords;
void main(){
	v_texCoords = a_pos;
	mat4 view = mat4(mat3(viewMatrix));
	vec4 pos = projMatrix * view * vec4(a_pos, 1.0f);
	if(u_preview == 1){
		gl_Position = u_viewProjection * u_transform * vec4(a_pos,1.0f);
	}else{
		gl_Position = pos.xyww;
	}
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;
in vec3 v_texCoords;
layout(binding = 1) uniform samplerCube skybox;
void main() {
	color = texture(skybox, v_texCoords);
}