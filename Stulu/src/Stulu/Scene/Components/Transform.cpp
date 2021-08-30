#include "st_pch.h"
#include "Transform.h"

#include <imgui.h>

#include "Stulu/ImGui/StuluImGui.h"

namespace Stulu {
	//////////////////////////Transform/////////////////////////////////////
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
	void Transform::drawImGui() {


		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Position_3d_Transform", glm::value_ptr(position), 3, .1f, 0, 0, "%.3f");

		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Rotation_3d_Transform", glm::value_ptr(rotation), 3, .1f, 0, 0, "%.3f");


		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Scale_3d_Transform", glm::value_ptr(scale), 3, .1f, 0, 0, "%.3f");
		if (ImGui::Button("Reset Transform")) {
			reset();
		}

	}
	//////////////////////////Transform/////////////////////////////////////
}