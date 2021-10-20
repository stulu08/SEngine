#include "Layer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

void DefaultLayer::onAttach() {
	shader = Shader::create("assets/test.glsl");
}

void DefaultLayer::onUpdate(Timestep ts) {
	
}
void DefaultLayer::onImguiRender(Timestep ts) {
	
}

void DefaultLayer::onEvent(Event& e) {
}
