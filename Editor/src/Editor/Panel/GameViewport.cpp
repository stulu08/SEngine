#include "GameViewport.h"
#include "Editor/Resources.h"
#include "ComponentsRender.h"
#include "Editor/EditorApp.h"

namespace Stulu {
	void GameViewportPanel::draw(const Ref<FrameBuffer>& frameBuffer, bool* open) {
		ST_PROFILING_FUNCTION();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		if (ImGui::Begin("Game", open, ImGuiWindowFlags_HorizontalScrollbar)) {
			if(ComponentsRender::drawComboControl("Sizes", m_selectedSize,"1920x1080\0 1000x1000\0 1280x720\0 500x500\0 250x250\0 Custom"))
				getEditorLayer().savePanelConfig();

			ComponentsRender::drawFloatSliderControl("Size Multiply",zoom,.0f,1.0f);
			if (m_selectedSize == 5) {
				glm::vec2 vec = { sizes[5].x, sizes[5].y};
				if (ComponentsRender::drawVector2Control("Size", vec)) {
					sizes[5] = { vec.x,vec.y };
					getEditorLayer().savePanelConfig();
				}
			}
			if(ImGui::BeginChild("Viewport",ImVec2(0,0),true, ImGuiWindowFlags_HorizontalScrollbar)) {
				Ref<Texture> viewPortTexture;
				if (frameBuffer == nullptr)
					viewPortTexture = EditorResources::getNoCameraTexture();
				else {
					if (frameBuffer->getTexture() == nullptr)
						viewPortTexture = EditorResources::getNoCameraTexture();
					else
						viewPortTexture = frameBuffer->getTexture();
				}
				ImVec2 viewportSize = sizes[m_selectedSize];


				width = (uint32_t)viewportSize.x;
				height = (uint32_t)viewportSize.y;
				viewportSize = viewportSize * zoom;
				ImGui::SetCursorPos((ImGui::GetWindowSize() - viewportSize) * 0.5f);
				ImGui::Image(reinterpret_cast<void*>((uint64_t)viewPortTexture->getRendererID()), viewportSize, ImVec2(0, 1), ImVec2(1, 0),ImVec4(1,1,1,1),ImGui::GetStyleColorVec4(ImGuiCol_Separator));

				bool bFocused = focused || hovered;
				focused = ImGui::IsWindowFocused();
				hovered = ImGui::IsItemHovered();
				windowPos = *((glm::vec2*)&ImGui::GetItemRectMin());
				if (!(focused || hovered) && bFocused && Input::getCursorMode() != Input::CursorMode::Normal) {
					Input::setCursorMode(Input::CursorMode::Normal);
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
	void GameViewportPanel::drawErrorImage(){
	}
}