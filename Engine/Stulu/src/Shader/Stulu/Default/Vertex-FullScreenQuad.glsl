#type Vertex
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoords;

layout (location = 0) out vec2 v_tex;

layout(std140, binding = 1) uniform model {
	float z;
};

void main() {
	v_tex = a_texCoords;
	gl_Position = vec4(a_pos.x, a_pos.y, z, 1.0);
}