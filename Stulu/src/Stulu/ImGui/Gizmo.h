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
		static void init();
		static void Begin();
		static void End();
		static void setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void setRect(const float x, const float y, const float width, const float height);
		static void ApplyToFrameBuffer(const Ref<FrameBuffer>& camera);

		static bool IsUsing();

		static bool TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType = GizmoTransformEditMode::Translate);
		static void drawGrid(const glm::mat4& transform, float size);

		static void drawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f,1.0f,1.0f,1.0f));

		static void drawRect(const glm::vec3& position, const glm::vec2& scale = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		static void drawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void drawOutlineCube(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		static void drawOutlineCube(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transform, const glm::vec4& color);

		static void drawTexture(const Ref<Texture>& texture, const glm::vec3& position, const glm::quat& rotation, const glm::vec2& scale = glm::vec2(1.0f), const glm::vec4& color = COLOR_WHITE_VEC4);
		static void drawTextureBillBoard(const Ref<Texture>& texture, const glm::vec3& position, const glm::vec2& scale = glm::vec2(1.0f), const glm::vec3& up = { 0, 1, 0 }, const glm::vec4& color = COLOR_WHITE_VEC4);
		
		static void drawCircle(const glm::vec3& position, const glm::vec2& scale, const glm::quat& rotation , const glm::vec4& color = COLOR_WHITE_VEC4, float thickness = 1.0f, float fade = .005f);
		static void drawCircleBillBoard(const glm::vec3& position, const glm::vec2& scale = glm::vec2(1.0f), const glm::vec4& color = COLOR_WHITE_VEC4, float thickness = 1.0f, float fade = .005f, const glm::vec3& up = { 0, 1, 0 });

		static void drawCube(const glm::mat4& transformation, const glm::vec4& color = COLOR_WHITE_VEC4);
		static void drawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const glm::vec4& color = COLOR_WHITE_VEC4);

		static void drawSphere(const glm::mat4& transformation, const glm::vec4& color = COLOR_WHITE_VEC4);
		static void drawSphere(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const glm::vec4& color = COLOR_WHITE_VEC4);

		static void drawGUIRect(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color = {1,1,1,1}, bool border = false, float overrideBorderSize = 0);
		static bool drawGUITextureButton(const Ref<Texture>& texture, const glm::vec2& position, 
			const glm::vec2& size = {50,50}, const glm::vec4& color = {1,1,1,1}, 
			const glm::vec2& uv1 = { 0,1 }, const glm::vec2& uv2 = { 1,0 }, 
			const int borderSize = 1.0f, const glm::vec4& bgColor = {0,0,0,0});


	private:
		static void resetCubes();
		static void resetSpheres();
		static void flushCubes();
		static void flushSpheres();
	};
}