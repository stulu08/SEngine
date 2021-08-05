##type vertex
#version 460 core
layout(location = 0) in vec3 a_pos;
void main(){
	gl_Position = vec4(a_pos, 1.0);
}
##type fragment
#version 460 core
layout(location = 0) out vec4 color;
uniform vec4 u_color;
void main(){
	color = u_color;
}
