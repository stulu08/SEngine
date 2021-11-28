#pragma once
#include "Stulu/Core/Layer.h"
struct ImVec2;
struct ImGuiContext;

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


