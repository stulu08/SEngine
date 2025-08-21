#pragma once
#include "Stulu/Math/Math.h"

namespace Stulu {
	struct alignas (16) FogSettings {
		glm::vec4 fogColor = glm::vec4(0.44f, 0.44f, 0.44f, 1.0f);
		float fogMode = 0.0f; /* 0 = disbaled, 1 = linear, 2 = exp, 3 = exp2  */
		float linearFogStart = 50.0f;
		float linearFogEnd = 250.0f;
		float fogDensity = 0.005f;
		float FogHorizonStrength = 0.0f;
		float FogHorizonFalloff = 15.0f;
		float EnableFogHorizon = 1.0f;
		float FogGroundStrength = 1.0f;
		float EnableGroundFog = 0.0f;
		float FogHorizonOffset = -1000.0f;
		float FogHorizonHeightFalloff = 2000.0f;
		float EnableHorizonHeightFog = 1.0f;
		enum class Mode {
			Disabled = 0,
			Linear = 1,
			Exp = 2,
			Exp2 = 3
		};
	};
	struct alignas (16) SceneBufferData {
		glm::mat4 skyBoxRotation = glm::mat4(1.0f);
		glm::vec4 clearColor = glm::vec4(.0f);

		// 4 scalars
		uint32_t useSkybox = 0;
		uint32_t shaderFlags = 0;
		float EnvironmentStrength = 1.0f;
		float EnvironmentReflections = 1.0f;

		glm::mat4 lightSpaceMatrices[ST_MAX_SHADOW_CASCADES + 1] = {};
		glm::vec4 cascadePlaneDistances[ST_MAX_SHADOW_CASCADES + 1] = {};
		glm::vec4 ShadowSettingsPack = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // blendDistance, Poisson Quality, CascadeCount
		glm::vec4 ShadowSettingsPack2 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		FogSettings fogSettings;
	};
}