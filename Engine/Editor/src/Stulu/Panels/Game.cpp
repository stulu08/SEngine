#include "Game.h"
#include "Stulu/App.h"

#include <Stulu/Input/Input.h>
#include <Stulu/Scripting/Managed/Bindings/Core/Input.h>

#include "Stulu/MainLayer.h"

using namespace Stulu;

namespace Editor {
	GamePanel::GamePanel() 
		: Panel("Game") {

		FrameBufferSpecs specs;
		specs.width = 1;
		specs.height = 1;
		m_framebuffer = FrameBuffer::create(specs);
	}

	void GamePanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Game Panel");

		m_hovered = ImGui::IsWindowHovered();

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_width = (uint32_t)glm::max(viewportSize.x, 1.0f);
		m_height = (uint32_t)glm::max(viewportSize.y, 1.0f);
		UpdateFrameBuffer(m_width, m_height);

		m_windowPos = ImGui::GetCursorScreenPos();

		auto& texture = m_framebuffer->getColorAttachment();
		ImGui::Image(texture.get(), viewportSize, ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
	}

	void GamePanel::Update() {
		auto& layer = App::get().GetLayer();

		// if anywhere outside of window return to defualt cursor mode
		if (!IsFocused()) {
			Input::setCursorMode(Input::CursorMode::Normal);
		}

		// enable input during runtime, if the window is focused and hovered, unless mouse is disabled, then skip hover check
		StuluBindings::Input::SetEnabled(
			IsFocused() && m_hovered && layer.IsRuntime());

		if (layer.IsRuntime()) {
			layer.GetActiveScene()->onUpdateRuntime(IsVisible());
			if (IsVisible()) {
				layer.GetActiveScene()->getRenderer()->GenSceneTexture(GetFrameBuffer());
			}
		}

		m_hovered = false;
	}

	void GamePanel::OnEvent(Stulu::Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(GamePanel::OnKeyPressed));
	}

	void GamePanel::PreWindow() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	}

	void GamePanel::PostWindow() {
		ImGui::PopStyleVar();

		if (!IsVisible())
			return;

		App::get().GetLayer().GetPanel<ScenePanel>().DrawMenuBars(m_windowPos, false, (float)m_width);
	}


	bool GamePanel::OnKeyPressed(Stulu::KeyDownEvent& e) {
		if (e.getKeyCode() == Keyboard::Escape) {
			Input::setCursorMode(Input::CursorMode::Normal);
			return true;
		}
		return false;
	}

	void GamePanel::UpdateFrameBuffer(uint32_t width, uint32_t height) {
		FrameBufferSpecs specs = m_framebuffer->getSpecs();
		if (specs.width != width || specs.height != height) {
			m_framebuffer->resize(width, height);

			auto& layer = App::get().GetLayer();
			if(layer.GetActiveScene())
				layer.GetActiveScene()->onViewportResize(width, height);
		}
	}
}