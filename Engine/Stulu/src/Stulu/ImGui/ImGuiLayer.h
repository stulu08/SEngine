#pragma once
#include "Stulu/Core/Layer.h"
#include <Stulu/Renderer/Texture.h>
struct ImVec2;
struct ImVec4;
struct ImGuiContext;
struct ImDrawList;
struct ImGuiStyle;
#ifndef ImTextureID
typedef void* ImTextureID;          // Default: store a pointer or an integer fitting in a pointer (most renderer backends are ok with that)
#endif

STULU_API ImVec2 operator *(const ImVec2& vec, const float v);
STULU_API ImVec2 operator /(const ImVec2& vec, const float v);
STULU_API ImVec2 operator +(const ImVec2& vec, const float v);
STULU_API ImVec2 operator -(const ImVec2& vec, const float v);
STULU_API ImVec2 operator *(const ImVec2& vec, const ImVec2& vec2);
STULU_API ImVec2 operator /(const ImVec2& vec, const ImVec2& vec2);
STULU_API ImVec2 operator +(const ImVec2& vec, const ImVec2& vec2);
STULU_API ImVec2 operator -(const ImVec2& vec, const ImVec2& vec2);
STULU_API ImVec2 operator *(const ImVec2& vec, const glm::vec2& vec2);
STULU_API ImVec2 operator /(const ImVec2& vec, const glm::vec2& vec2);
STULU_API ImVec2 operator +(const ImVec2& vec, const glm::vec2& vec2);
STULU_API ImVec2 operator -(const ImVec2& vec, const glm::vec2& vec2);
namespace ImGui {
	STULU_API void Image(const Stulu::Ref<Stulu::Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col);
	STULU_API void Image(const Stulu::Ref<Stulu::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0 = { 0,0 }, const glm::vec2& uv1 = { 1,1 }, const glm::vec4& tint_col = { 1,1,1,1 }, const glm::vec4& border_col = { 0,0,0,0 });

	STULU_API bool ImageButton(const Stulu::Ref<Stulu::Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col);    // <0 frame_padding uses default frame padding settings. 0 for no padding
	STULU_API bool ImageButton(const Stulu::Ref<Stulu::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0, 0), const glm::vec2& uv1 = glm::vec2(1, 1), int frame_padding = -1, const glm::vec4& bg_col = glm::vec4(0, 0, 0, 0), const glm::vec4& tint_col = glm::vec4(1, 1, 1, 1));    // <0 frame_padding uses default frame padding settings. 0 for no padding

	STULU_API ImTextureID StuluTextureToImGui(const Stulu::Ref<Stulu::Texture>& texture);

	STULU_API void StyleColorsOceanDark(ImGuiStyle* dst = NULL);
	STULU_API void StyleColorsAmoledDark(ImGuiStyle* dst = NULL);
	STULU_API void StyleColorsGrayDark(ImGuiStyle* dst = NULL);
}
namespace Stulu {
	class STULU_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImguiRender(Timestep timestep) override;
		virtual void onEvent(Event& e) override;
		void blockEvents(bool blockEvents) { m_blockEvents = blockEvents; }

		void Begin();
		void End();


		inline ImGuiContext* getContext() { return m_context; }
	private:
		bool m_blockEvents = true;
		float m_time = 0.0f;
		ImGuiContext* m_context;
	};
}


