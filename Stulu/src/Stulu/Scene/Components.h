#pragma once
#include "Stulu/ImGui/StuluImGui.h"
#include <Stulu/Renderer/Model.h>
#include "Stulu/Scene/Components/Transform.h"

namespace Stulu {
	class Component {
	public:
		virtual void onUpdate() { };
		virtual void drawImGui() = 0;
		virtual const char* getTypeName() = 0;
		Transform& transform = Transform();
	};
	class PBRShaderEdit : public Component {
	public:
		PBRShaderEdit(Ref<Shader>& shader)
			: m_shader(shader) {};
		virtual ~PBRShaderEdit() = default;

		virtual void onUpdate() override {
			m_shader->bind();
			m_shader->setFloat3("u_albedo",albedo);
			m_shader->setFloat("u_metallic",metallic);
			m_shader->setFloat("u_roughness",roughness);
			m_shader->setFloat("u_ao",ao);
			m_shader->unbind();
		}

		virtual void drawImGui() override;
		virtual const char* getTypeName() override {
			return "PBRShaderEdit";
		}
		glm::vec3 albedo = glm::vec3(1.0f);
		float metallic = .5f;
		float roughness = .5f;
		float ao = .2f;
	private:
		Ref<Shader> m_shader;
	};
	class ModelRenderer : public Component {
	public:
		ModelRenderer(Model& model, Ref<Shader>& shader)
			: m_model(model), m_shader(shader) {};
		virtual ~ModelRenderer() = default;

		virtual void onUpdate() override {
			m_model.submitToRenderer(m_shader, transform);
		}

		virtual void drawImGui() override;
		virtual const char* getTypeName() override {
			return "ModelRenderer";
		}
	private:
		Ref<Shader> m_shader;
		Model m_model;
	};
	class Light : public Component {
	public:
		Light() {  };
		virtual ~Light() = default;

		virtual void onUpdate() override {
			
		}

		virtual void drawImGui() override;
		virtual const char* getTypeName() override {
			return "Light";
		}
		
		float strength = 1.0f;
		glm::vec3 color = {1.0f,1.0f,.6f};
	};
}