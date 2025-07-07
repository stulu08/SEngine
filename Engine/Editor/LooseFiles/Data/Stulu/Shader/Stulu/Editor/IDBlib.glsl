#SShader "Editor/IDBlib"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment

#include "Stulu/Buffer/FullscreenQuadBuffer.glsl"

layout (location = 0) out uint resolvedOut;

layout (location = 0) in vec2 v_tex;
layout (binding = 0) uniform sampler2DMS screenTextureMS;


void main()
{
    ivec2 texturePos = ivec2(round(pixelWidth * v_tex.x), round(pixelHeight * v_tex.y));
    float pixelR = texelFetch(screenTextureMS, texturePos, 0).r;
    resolvedOut = uint(pixelR);
}