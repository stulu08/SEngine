#include "st_pch.h"
#include "Gizmo.h"

#include <Imgui.h>
#include <ImGuizmo.h>

namespace Stulu {
	Gizmo::Data Gizmo::s_data;

	bool Gizmo::TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType) {
		ST_PROFILING_FUNCTION();
		if (gizmoEditType == GizmoTransformEditMode::None)
			return false;
		glm::mat4 transform = tc.getWorldSpaceTransform();
		ImGuizmo::Manipulate(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix),
			(ImGuizmo::OPERATION)((int)gizmoEditType), ImGuizmo::MODE::LOCAL, glm::value_ptr(transform));

		if (ImGuizmo::IsUsing()) {
			if (tc.parent) {
				glm::vec3 worldPos, worldRotation, worldScale;
				Math::decomposeTransform(transform, worldPos, worldRotation, worldScale);
				glm::mat4 parent = tc.parent.getComponent<TransformComponent>().getWorldSpaceTransform();
				const glm::vec3 parentPos = glm::vec3(parent[3]);
				parent[3] = glm::vec4(0, 0, 0, parent[3].w);
				transform = glm::translate(glm::mat4(1.0f), worldPos - parentPos) * glm::toMat4(glm::quat(worldRotation)) * glm::scale(glm::mat4(1.0f), worldScale) / parent;
			}
			glm::vec3 pos, rotation, scale;
			Math::decomposeTransform(transform, pos, rotation, scale);

			tc.position = pos;
			tc.rotation = rotation;
			tc.scale = scale;
			return true;
		}
		return false;
	}
	void Gizmo::drawGrid(const glm::mat4& matrix, const float& size) {
		ST_PROFILING_FUNCTION();
		ImGuizmo::DrawGrid(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix), glm::value_ptr(matrix), size);
	}
	void Gizmo::setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView) {
		ST_PROFILING_FUNCTION();
		s_data.projMatrix = cameraProjection;
		s_data.viewMatrix = cameraView;
	}
	void Gizmo::setRect(const glm::vec2 pos, float width, float height) {
		ST_PROFILING_FUNCTION();
		ImGuizmo::SetRect(pos.x, pos.y, width, height);
	}
}