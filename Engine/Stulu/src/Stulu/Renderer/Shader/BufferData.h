#pragma once
#include "Stulu/Math/Math.h"

namespace Stulu {
	struct alignas (16) CameraBufferData {
		glm::mat4 viewProjectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::vec4 cameraPosition;
		glm::vec4 cameraRotation;
		glm::vec4 cameraNearFar;
	};
	struct alignas (16) LightBufferData {
		struct Light {
			glm::vec4 ColorStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			glm::vec4 PositionType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
			glm::vec4 Direction = glm::vec4(90.0f, .0f, .0f, 1.0f);
			glm::vec4 Data = glm::vec4(1.0f);
			glm::vec4 ShadowData = glm::vec4(0.0f);
		} lights[ST_MAXLIGHTS];

		uint32_t lightCount = 0;
	};
	struct alignas (16) GlobalMaterialBufferData {
		uint32_t alphaMode;
		float alphaCutOff;
	};

	struct alignas (16) StaticModelDataBuffer {
		// will reinterpreted as uint32_t, for performance gain float is used here, due to some weird x86 cpu instructions (test: +- 15fps), also std140 layout rules
		glm::vec4 entityID;
		glm::mat4 normal;
		glm::mat4 transform;
	};
	struct alignas (16) InstancedModelDataBuffer {
		glm::vec4 entityID;
		glm::mat4 instanceMatrices[ST_MAX_INSTANCES];

		inline void SetNormalMatrix(size_t index, const glm::mat4& norm) {
			instanceMatrices[index * 2] = norm;
		}
		inline void SetTranslationMatrix(size_t index, const glm::mat4& trans) {
			instanceMatrices[index * 2 + 1] = trans;
		}
		inline void SetNormalFromTranslation(size_t index, const glm::mat4& trans) {
			instanceMatrices[index * 2] = glm::transpose(glm::inverse(trans));
		}
	};
	struct alignas (16) SkinnedModelDataBuffer {
		glm::vec4 entityID;
		glm::mat4 boneMatrices[ST_MAX_BONES];
	};
	struct alignas (16) SharedModelDataBuffer {
		union {
			StaticModelDataBuffer staticData;
			InstancedModelDataBuffer instancedData;
			SkinnedModelDataBuffer skinnedData;
		};
	};
}