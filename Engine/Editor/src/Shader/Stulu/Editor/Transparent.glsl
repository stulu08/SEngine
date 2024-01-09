#SShader "Editor/Transparent"

#include "Stulu/Default/Vertex.glsl"

#type fragment
#version 460 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}