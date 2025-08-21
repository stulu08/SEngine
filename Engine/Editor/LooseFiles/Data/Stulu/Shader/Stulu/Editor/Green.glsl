#SShader "Editor/Green"

#include "Stulu/Default/Vertex.glsl"

#type fragment
layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}