##add ST_default
layout(binding = 2) uniform sampler2D u_texture;
void main() {
	FragColor = texture(u_texture, vertex.texCoords);
}