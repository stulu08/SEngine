#SShader "Editor/Transparent"

#include "Stulu/Default/Vertex.glsl"

#type fragment
layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}