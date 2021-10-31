#include "st_pch.h"
#include "Material.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/YAML.h"


namespace Stulu {
	Material Material::fromDataStringPath(const std::string& path, UUID uuid) {
		MaterialData data;
		YAML::Node YAMLdata = YAML::LoadFile(path);
		UUID shaderUuid = UUID(YAMLdata["Shader"].as<uint64_t>());
		data.materialStructName = YAMLdata["StructName"].as<std::string>();
		
		auto DataTypes = YAMLdata["DataTypes"];
		for (auto dataType : DataTypes) {
			MaterialDataType daty = dataType.as<MaterialDataType>();
			data.dataTypes.push_back(daty);
		}
		Material mat = Material(Asset{ AssetType::Unknown, 0, AssetsManager::get(shaderUuid).path, shaderUuid }, data);
		mat.m_uuid = uuid;
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		mat.m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);
		return mat;
	}
	Material::Material(Asset& shader, const MaterialData& data)
		: m_runtimeData(data) {
		m_shader = Shader::create(shader.path);
		m_shaderUuid = shader.uuid;
		m_uuid = UUID::null;
		uploadData();
	}
	void Material::toDataStringFile(std::string path) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << (uint64_t)m_shaderUuid;
		out << YAML::Key << "StructName" << YAML::Value << m_runtimeData.materialStructName;
		out << YAML::Key << "DataTypes" << YAML::Value << YAML::BeginSeq;
		for (auto& dataType : m_runtimeData.dataTypes) {
			out << dataType;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);
	}
	void Material::uploadData() {
		int texCount = 2;
		m_shader->bind();
		for (auto& dataType : m_runtimeData.dataTypes) {
			std::string name = m_runtimeData.materialStructName + "." + dataType.name;
			switch (dataType.type) {
				case ShaderDataType::Float:		m_shader->setFloat(name, std::any_cast<float>(dataType.data));			break;
				case ShaderDataType::Float2:	m_shader->setFloat2(name, std::any_cast<glm::vec2>(dataType.data));		break;
				case ShaderDataType::Float3:	m_shader->setFloat3(name, std::any_cast<glm::vec3>(dataType.data));		break;
				case ShaderDataType::Float4:	m_shader->setFloat4(name, std::any_cast<glm::vec4>(dataType.data));		break;
																											
				case ShaderDataType::Int:		m_shader->setInt(name, std::any_cast<int>(dataType.data));				break;
				//case ShaderDataType::Int2:		m_shader->setInt2()										break;
				//case ShaderDataType::Int3:		m_shader->set()											break;
				//case ShaderDataType::Int4:		m_shader->set()											break;
																											
				//case ShaderDataType::Mat3:		m_shader->setMat3()										break;
				case ShaderDataType::Mat4:		m_shader->setMat4(name, std::any_cast<glm::mat4>(dataType.data));		break;
																											
				//case ShaderDataType::Bool:		m_shader->setBool()										break;
				case ShaderDataType::Sampler:
					m_shader->setInt(dataType.name, texCount);
					texCount++;
					break;
				default:
					CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
			}
		}
	}
	void Material::bind() {
		int texCount = 2;
		for (auto& dataType : m_runtimeData.dataTypes) {
			if (dataType.type == ShaderDataType::Sampler) {
				Asset& asset = AssetsManager::get(std::any_cast<UUID>(dataType.data));

				switch (asset.type)
				{
				case Stulu::AssetType::Texture2D:
					asset.data._Cast<Ref<Texture2D>>()->get()->bind(texCount);
					break;
				case Stulu::AssetType::Texture:
					asset.data._Cast<Ref<Texture>>()->get()->bind(texCount);
					break;
				case Stulu::AssetType::CubeMap:
					asset.data._Cast<Ref<CubeMap>>()->get()->bind(texCount);
					break;
				default:
					break;
				}
				texCount++;
				break;
			}
		}
		m_shader->bind();
	}
	void Material::unbind(){
		m_shader->unbind();
		for (auto& dataType : m_runtimeData.dataTypes) {
			switch (dataType.type) {
				case ShaderDataType::Sampler:
					m_shader->setInt(dataType.name, 0);
					break;
			}
		}
	}
	bool Material::operator==(const Material& other) const {
		bool samePath = m_uuid == other.m_uuid;
		bool sameShaderPath = m_shaderUuid == other.m_shaderUuid;
		bool sameDataStructName = m_runtimeData.materialStructName == other.m_runtimeData.materialStructName;
		bool sameData = m_runtimeData.dataTypes == other.m_runtimeData.dataTypes;

		return sameData && sameShaderPath && sameDataStructName && sameData;
	}
	bool MaterialDataType::operator==(const MaterialDataType& other) const {
		if (!(type == other.type && name == other.name))
			return false;

		bool sameData = false;
		switch (type) {
			case ShaderDataType::Float:		sameData = std::any_cast<float>(data) == std::any_cast<float>(other.data);				break;
			case ShaderDataType::Float2:	sameData = std::any_cast<glm::vec2>(data) == std::any_cast<glm::vec2>(other.data);		break;
			case ShaderDataType::Float3:	sameData = std::any_cast<glm::vec3>(data) == std::any_cast<glm::vec3>(other.data);		break;
			case ShaderDataType::Float4:	sameData = std::any_cast<glm::vec4>(data) == std::any_cast<glm::vec4>(other.data);		break;

			case ShaderDataType::Int:		sameData = std::any_cast<int>(data) == std::any_cast<int>(other.data);					break;
			//case ShaderDataType::Int2:	m_shader->setInt2()										break;
			//case ShaderDataType::Int3:	m_shader->set()											break;
			//case ShaderDataType::Int4:	m_shader->set()											break;

			//case ShaderDataType::Mat3:	m_shader->setMat3()										break;
			case ShaderDataType::Mat4:		sameData = std::any_cast<glm::mat4>(data) == std::any_cast<glm::mat4>(other.data);		break;

			//case ShaderDataType::Bool:	m_shader->setBool()										break;
			case ShaderDataType::Sampler:	sameData = std::any_cast<UUID>(data) == std::any_cast<UUID>(other.data);				break;
		}


		return sameData;

	}
}