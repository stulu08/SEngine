#ifndef _STULU_MESH_LAYOUT_GLSL_
#define _STULU_MESH_LAYOUT_GLSL_
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in vec4 a_color;
layout (location = 4) in vec4 a_boneWeights;
layout (location = 5) in ivec4 a_boneIDs;
#endif