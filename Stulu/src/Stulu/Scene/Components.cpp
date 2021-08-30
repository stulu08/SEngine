#include "st_pch.h"
#include "Components.h"
#include <imgui.h>

namespace Stulu {
	void ModelRenderer::drawImGui() {
		ImGui::Text("Triangles: %d", m_model.getTriangleCount());
		ImGui::Text("Verticess: %d", m_model.getVerticesCount());
		ImGui::Text("Indicess: %d", m_model.getIndicesCount());
		ImGui::Text("Shader: %s", m_shader->getName().c_str());
	}
	void Light::drawImGui() {
		ImGui::ColorEdit3("Color", glm::value_ptr(color));
		imGui::Float("Strength", strength);
	}
	void PBRShaderEdit::drawImGui() {
		ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo));
		imGui::SliderFloat("Metallic", &metallic);
		imGui::SliderFloat("Roughness", &roughness);
		imGui::SliderFloat("AO", &ao);
	}
}