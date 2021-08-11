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
		/*return glm::translate(glm::mat4(1.0f), m_pos)
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), m_scale);*/
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
		if(begin)
			ImGui::Begin(name);
		ImGui::Text("Transform");
		if (_2D) {
			ImGui::DragFloat2("Position", glm::value_ptr(transform.getPos()), .1f);
			ImGui::DragFloat2("Size", glm::value_ptr(transform.getScale()), .1f);
			ImGui::DragFloat("Rotation", &transform.getRotation().z);
		}
		else {
			ImGui::DragFloat3("Position", glm::value_ptr(transform.getPos()), .1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transform.getRotation()));
			ImGui::DragFloat3("Scale", glm::value_ptr(transform.getScale()), .1f);
		}
		if (ImGui::Button("Reset Transform"))
			transform.reset();
		if(end)
			ImGui::End();
	}
}