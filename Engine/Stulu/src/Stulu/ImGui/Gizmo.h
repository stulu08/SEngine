#pragma once
#include <Stulu.h>
#include <Stulu/Scene/Components/Components.h>
namespace Stulu {
	enum GizmoTransformEditMode : uint32_t {
		None = 0,

		Translate_X = (1u << 0),
		Translate_Y = (1u << 1),
		Translate_Z = (1u << 2),
		Rotate_X = (1u << 3),
		Rotate_Y = (1u << 4),
		Rotate_Z = (1u << 5),
		Rotate_Screen = (1u << 6),//the white thing when rotating
		Scale_X = (1u << 7),
		Scale_Y = (1u << 8),
		Scale_Z = (1u << 9),
		Bounds = (1u << 10),
		Scale_XU = (1u << 11),//universal
		Scale_YU = (1u << 12),//universal
		Scale_ZU = (1u << 13),//universal

		Translate = Translate_X | Translate_Y | Translate_Z,
		Rotate = Rotate_X | Rotate_Y | Rotate_Z | Rotate_Screen,
		Scale = Scale_X | Scale_Y | Scale_Z,
		ScaleEu = Scale_XU | Scale_YU | Scale_ZU, // universal
		Universal = Translate | Rotate | ScaleEu
	};
	class STULU_API Gizmo {
	public:
		static void init();
		static bool Begin();
		// always call end
		static void End();
		static void setWindow(const std::string& windowName, bool* windowOpenPtr = nullptr);
		static void setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void setRect(const float x, const float y, const float width, const float height);
		static void ApplyToFrameBuffer(const Ref<FrameBuffer>& camera);

		static bool IsUsing();

		static bool TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType = GizmoTransformEditMode::Translate, const glm::vec3& snap = glm::vec3(.0f));
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