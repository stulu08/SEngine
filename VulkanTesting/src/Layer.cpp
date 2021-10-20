#include "Layer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

void DefaultLayer::onAttach() {
	
}

void DefaultLayer::onUpdate(Timestep ts) {
	
}
void DefaultLayer::onImguiRender(Timestep ts) {
	ImGui::Begin("Debug");

	ImGui::Text("Metallic %.5f", std::any_cast<float>(mat.getData().dataTypes[0].data));
	if (ImGui::Button("Save")) {
		mat.toDataStringFile("assets/material.mat");
	}
	if (ImGui::Button("Open")) {
		mat = Material::fromDataStringPath("assets/material.mat");
	}
	ImGui::End();
}

void DefaultLayer::onEvent(Event& e) {
}
