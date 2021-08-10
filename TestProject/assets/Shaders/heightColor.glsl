##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
uniform mat4 u_viewProjection;
uniform mat4 u_transform;
uniform float u_maxHeight = 1.0f;
out float v_height;
void main(){
	v_height = 1 + a_pos.y / u_maxHeight;
	gl_Position = u_viewProjection * u_transform * vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;
in float v_height;
void main(){
	vec3 col = vec3(0.0f,0.0f,0.7f);
	if(v_height > 0.3f)
		col = vec3(0.1f,0.2f,0.8f);
	if(v_height > 0.6f)
		col = vec3(0.3f,0.4f,0.8f);
	if(v_height > 0.8f)
		col = vec3(0.4f,0.5f,0.8f);
	if(v_height > 1.0f)
		col = vec3(0.9f,0.9f,0.04f);
	if(v_height > 1.1f)
		col = vec3(0.1f,0.7f,0.2f);
	if(v_height > 1.6f)
		col = vec3(0.2f,0.2f,0.2f);
	if(v_height > 1.95f)
		col = vec3(0.8f,0.9f,0.8f);
	
	color = vec4(col * v_height, 1.0f);

}