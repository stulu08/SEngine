#pragma once
#include <Stulu.h>

namespace Stulu {
	enum class GizmoTransformEditMode {
		None = -1,
		Translate = 0,
		Rotate = 1,
		Scale = 2,
	};
	class Gizmo {
	public:
		static bool TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType = GizmoTransformEditMode::Translate);
		static void drawGrid(const glm::mat4& matrix, const float& size);

		static void setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void setRect(const glm::vec2 pos, float width, float height);
	private:
		static struct Data {
			glm::mat4 viewMatrix;
			glm::mat4 projMatrix;
		}s_data;
	};
}