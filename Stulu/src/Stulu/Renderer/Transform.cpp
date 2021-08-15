#include "st_pch.h"
#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Stulu {
	const glm::quat Transform::getOrientation() {
		return glm::quat(glm::radians(rotation));
	}
	const glm::mat4 Transform::toMat4() {
		return glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(getOrientation())
			* glm::scale(glm::mat4(1.0f), scale);
	}
	void Transform::reset() {
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
}