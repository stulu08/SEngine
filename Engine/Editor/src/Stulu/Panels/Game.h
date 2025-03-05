#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Renderer/FrameBuffer.h>
#include <Stulu/Events/KeyEvent.h>
#include <Stulu/Events/MouseEvent.h>

namespace Editor {
	class GamePanel : public Panel {
	public:
		GamePanel();

		virtual void DrawImGui() override;
		virtual void Update() override;
		virtual void OnEvent(Stulu::Event& e) override;

		virtual void PreWindow() override;
		virtual void PostWindow() override;

		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }

		Stulu::Ref<Stulu::FrameBuffer> GetFrameBuffer() const {
			return m_framebuffer;
		}
	private:
		uint32_t m_width = 1, m_height = 1;
		ImVec2 m_windowPos;
		Stulu::Ref<Stulu::FrameBuffer> m_framebuffer;

		bool OnKeyPressed(Stulu::KeyDownEvent& e);

		void UpdateFrameBuffer(uint32_t width, uint32_t height);
	};
}