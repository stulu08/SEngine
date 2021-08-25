#include "st_pch.h"
#include "Transform.h"

namespace Stulu {
	const glm::quat Transform::getOrientation() {
		return glm::quat(glm::radians(rotation));
	}
	Transform::operator const glm::mat4() {
		if (rotation != glm::vec3(0.0f, 0.0f, 0.0f))
			return glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(getOrientation())
			* glm::scale(glm::mat4(1.0f), scale);

		return glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), scale);
	}
	void Transform::reset() {
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
	glm::vec3 Transform::upDirection() {
		return glm::rotate(getOrientation(), TRANSFORM_UP_DIRECTION);
	}

	glm::vec3 Transform::rightDirection() {
		return glm::rotate(getOrientation(), TRANSFORM_RIGHT_DIRECTION);
	}

	glm::vec3 Transform::forwardDirection() {
		return glm::rotate(getOrientation(), TRANSFORM_FOREWARD_DIRECTION);
	}
}