#pragma once
#include <Stulu.h>
#include <Stulu/Scene/Components/Components.h>
namespace Stulu {
	enum class GizmoTransformEditMode {
		None = -1,
		Translate = 0,
		Rotate = 1,
		Scale = 2,
	};
	class Gizmo {
	public:
		static void Begin();
		static void setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void setRect(const float x, const float y, const float width, const float height);

		static bool TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType = GizmoTransformEditMode::Translate);
		static void drawGrid(const glm::mat4& matrix, const float& size);

		static void drawLine(const glm::vec3& begin, const glm::vec3 end, const float& thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f,1.0f,1.0f,1.0f));

		static void drawRect(const glm::vec3& position, const glm::vec2& scale = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		static void drawRect(const glm::mat4& transform, const float& thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void drawOutlineCube(const glm::mat4& transform, const float& thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	private:
		static glm::vec2 worldToPos(const glm::vec3& worldPos);
		
		
		static struct Data {
			glm::mat4 viewMatrix;
			glm::mat4 projMatrix;

			float mX = 0.f;
			float mY = 0.f;
			float mWidth = 0.f;
			float mHeight = 0.f;
			float mXMax = 0.f;
			float mYMax = 0.f;
			float mDisplayRatio = 1.f;
            ImDrawList* mDrawList;
		}s_data;
	};
}