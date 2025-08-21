#include "st_pch.h"
#include "TestMaterial.h"

#include "MaterialProperty.h"
#include "Stulu/Renderer/ShaderSystem.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	TestMaterial::TestMaterial(Ref<ShaderEntry> shaderEntry, const std::string& name)
		: m_shader(shaderEntry->GetShader()), m_name(name) {
		SetShader(shaderEntry);
	}
	void TestMaterial::SetData(size_t offset, size_t size, const void* data) {
		m_buffer->setData(data, (uint32_t)size, (uint32_t)offset);
	}

	void TestMaterial::RenderReady() {
		GlobalMaterialBufferData data{ (uint32_t)m_transparencyMode, m_cutOut };
		Renderer::uploadBufferData(BufferBinding::Material, &data, sizeof(data));

		for (auto& prop : m_properties) {
			const MaterialPropertyType type = prop->GetType();
			if (MaterialProperty::IsTextureType(type))
				prop->ApplyValue(this);
		}

		m_buffer->bind((uint32_t)BufferBinding::UserMaterial);
	}

	void TestMaterial::SetShader(Ref<ShaderEntry> shaderEntry) {
		m_shader = shaderEntry->GetShader();
		m_properties.clear();
		m_buffer.reset();

		size_t materialBufferSize = 0;

		for (const auto& prop : shaderEntry->GetProperities()) {
			Ref<MaterialProperty> property = nullptr;
			switch (prop->GetType())
			{
			case Stulu::MaterialPropertyType::Float:
				property = createRef<MaterialFloatProperty>((*std::dynamic_pointer_cast<MaterialFloatProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::Float2:
				property = createRef<MaterialFloat2Property>((*std::dynamic_pointer_cast<MaterialFloat2Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::Float3:
				property = createRef<MaterialFloat3Property>((*std::dynamic_pointer_cast<MaterialFloat3Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::Float4:
				property = createRef<MaterialFloat4Property>((*std::dynamic_pointer_cast<MaterialFloat4Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::Int:
				property = createRef<MaterialIntProperty>((*std::dynamic_pointer_cast<MaterialIntProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::Int2:
				property = createRef<MaterialInt2Property>((*std::dynamic_pointer_cast<MaterialInt2Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::Int3:
				property = createRef<MaterialInt3Property>((*std::dynamic_pointer_cast<MaterialInt3Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::Int4:
				property = createRef<MaterialInt4Property>((*std::dynamic_pointer_cast<MaterialInt4Property>(prop)));
				break;
			case Stulu::MaterialPropertyType::UInt:
				property = createRef<MaterialUIntProperty>((*std::dynamic_pointer_cast<MaterialUIntProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::Sampler2D:
				property = createRef<MaterialSampler2DProperty>((*std::dynamic_pointer_cast<MaterialSampler2DProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::Cubemap:
				property = createRef<MaterialSamplerCubeMapProperty>((*std::dynamic_pointer_cast<MaterialSamplerCubeMapProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::SkyBox:
				property = createRef<MaterialSkyBoxProperty>((*std::dynamic_pointer_cast<MaterialSkyBoxProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::Color:
				property = createRef<MaterialColorProperty>((*std::dynamic_pointer_cast<MaterialColorProperty>(prop)));
				break;
			case Stulu::MaterialPropertyType::None:
			default:
				continue;
			}
			m_properties.push_back(property);

			const size_t thisSize = property->GetSize() + prop->GetOffset();
			materialBufferSize = glm::max(materialBufferSize, thisSize);
		}
		m_buffer = UniformBuffer::create((uint32_t)materialBufferSize, (uint32_t)BufferBinding::UserMaterial);
	}

	bool TestMaterial::HasProperity(const std::string& name) const {
		for (auto& prop : m_properties) {
			if (prop->GetName() == name) {
				return true;
			}
		}
		return false;
	}

	Ref<MaterialProperty> TestMaterial::GetProperity(const std::string& name) const {
		for (auto& prop : m_properties) {
			if (prop->GetName() == name) {
				return prop;
			}
		}
		return nullptr;
	}
}