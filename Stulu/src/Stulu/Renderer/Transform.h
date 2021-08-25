#pragma once
#include "Stulu/Math/Math.h"
namespace Stulu {
	struct Transform {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		Transform() {  };
		Transform(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale)
			: position(pos), rotation(rotation), scale(scale) {};

		inline void setRotation(float x, float y, float z) { rotation = glm::degrees(glm::vec3(x,y,z)); }

		const glm::quat getOrientation();

		operator const glm::mat4();

		void reset();

		glm::vec3 upDirection();
		glm::vec3 rightDirection();
		glm::vec3 forwardDirection();
	};
}