#include "st_pch.h"
#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Stulu {
	const glm::quat Transform::getOrientation() {
		return glm::quat(glm::radians(m_rotation));
	}
	const glm::mat4 Transform::toMat4() {
		return glm::translate(glm::mat4(1.0f), m_pos)
			* glm::toMat4(getOrientation())
			* glm::scale(glm::mat4(1.0f), m_scale);
	}
	void Transform::reset() {
		m_pos = glm::vec3(0.0f);
		m_rotation = glm::vec3(0.0f);
		m_scale = glm::vec3(1.0f);
	}
	void Transform::ImGuiTransformEdit(Transform& transform, const char* name, bool _2D, bool end, bool begin) {

	}
}