#include "st_pch.h"
#include "MaterialAsset.h"

#include "Stulu/Renderer/Material/MaterialProperty.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Stulu/Renderer/Renderer.h"

#include <magic_enum/magic_enum.hpp>

namespace Stulu {
	bool SharedMaterialAssetData::Load() {
		if (IsMemoryLoaded())
			return false;

		YAML::Node yaml = YAML::LoadFile(GetPath());

		Ref<ShaderEntry> shader;
		if (yaml["Shader"]) {
			shader = Renderer::getShaderSystem()->GetEntry(yaml["Shader"].as<std::string>());
		}
		if (!shader)
			return false;

		Ref<TestMaterial> material = createRef<TestMaterial>(shader);

		if (yaml["Instanced"])
			material->SetInstanced(yaml["Instanced"].as<bool>());
		if (yaml["TransparencyMode"])
			material->SetTransparencyMode((MaterialTransparencyMode)yaml["TransparencyMode"].as<uint32_t>());
		if (yaml["Cutout"])
			material->SetAlphaCutOut(yaml["Cutout"].as<float>());

		if (yaml["Properties"]) {
			for (const auto& prop : material->GetProperities()) {
				prop->Deserializer(yaml["Properties"]);
				prop->ApplyValue(material.get());
			}
		}
		m_material = material;
		return true;
	}
	bool SharedMaterialAssetData::Save() const {
		if (!m_material || IsMemoryLoaded())
			return false;

		const std::string& path = GetPath();
		std::ofstream stream(path);
		if (!stream.is_open()) {
			return false;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << m_material->GetShader()->getName();
		out << YAML::Key << "Instanced" << YAML::Value << m_material->GetInstanced();
		out << YAML::Key << "TransparencyMode" << YAML::Value << (uint32_t)m_material->GetTransparencyMode();
		out << YAML::Key << "Cutout" << YAML::Value << m_material->GetAlphaCutOut();

		out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		for (const auto& prop : m_material->GetProperities()) {
			prop->Serializ(out);
		}
		out << YAML::EndMap;
		out << YAML::EndMap;

		stream << out.c_str();
		stream.close();

		return true;
	}
}