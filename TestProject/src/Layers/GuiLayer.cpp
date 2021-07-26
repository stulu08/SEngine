#include "GuiLayer.h"



void GuiLayer::onUpdate(){
	
}
void GuiLayer::onEvent(Stulu::Event& e) {
	
}

void GuiLayer::drawRendererInfos() {
	ImGui::Begin(std::string(app->graphicDriverConstants.vendor + " Renderer").c_str(), NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text(app->graphicDriverConstants.renderer.c_str());
	ImGui::Text("Version %s", app->graphicDriverConstants.version.c_str());
	ImGui::Text("GLSL version %s", app->graphicDriverConstants.other.c_str());
	ImGui::End();

}
void GuiLayer::onImguiRender() {
	drawRendererInfos();
}