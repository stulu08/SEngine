#SShader "Editor/ArrayBlib"

#include "Stulu/Default/Vertex-FullScreenQuad.glsl"

#type Fragment
#include "Stulu/Buffer/FullscreenQuadBuffer.glsl"
layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 TexCoord;

layout (binding = 1) uniform sampler2DArray arrayMap;

void main() {
    vec3 texPos = vec3(TexCoord, UserData.x);
    float depth = texture(arrayMap, texPos).r;

    FragColor = vec4(vec3(depth), 1.0);
}