#ifndef _STULU_DEFAULT_MODEL_BUFFER_GLSL_
#define _STULU_DEFAULT_MODEL_BUFFER_GLSL_

#include "Stulu/Internals.glsl"
#include "Stulu/Branchless.glsl"

//mat4 normalMatrix = boneMatrices[0];
//mat4 transform = boneMatrices[1];
layout(std140, binding = ST_BUFFER_MODEL_BIND) uniform DefaultModelData
{
	ivec4 EntityID;
	mat4 ModelMatrices[ST_MAX_INSTANCES];
};

mat4 GetNormalMatrix() {
	return ModelMatrices[gl_InstanceID * 2];
}
mat4 GetTranslationMatrix() {
	return ModelMatrices[gl_InstanceID * 2 + 1];
}
mat4 GetBoneMatrix(int index) {
	return ModelMatrices[index];
}
uint GetEntityID() {
	return EntityID.x;
}

void CalculateVertexPositionAndNormal(inout vec4 position, inout vec3 normal) {
	// Skinned
	float totalWeight = a_boneWeights.x + a_boneWeights.y + a_boneWeights.z + a_boneWeights.w;
	for (int i = 0; i < 4; ++i) {
		int boneIndex = a_boneIDs[i];
		float weight = a_boneWeights[i];
		position += GetBoneMatrix(boneIndex) * vec4(a_pos, 1.0) * weight;
		normal += mat3(GetBoneMatrix(boneIndex)) * a_normal * weight;
	}

	// Branchless fallback to static transform if no weights
	float fallback = when_zero(totalWeight); // = 1.0 if static mesh
	position  += GetTranslationMatrix() * vec4(a_pos, 1.0) * fallback;
	normal += (mat3(GetNormalMatrix()) * a_normal) * fallback;
}

#endif