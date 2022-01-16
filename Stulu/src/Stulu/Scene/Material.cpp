#include "st_pch.h"
#include "Material.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/YAML.h"


namespace Stulu {
	Material Material::fromDataStringPath(const std::string& path, UUID uuid) {
		std::vector<MaterialDataType> data;
		YAML::Node YAMLdata = YAML::LoadFile(path);
		UUID shaderUuid = UUID(YAMLdata["Shader"].as<uint64_t>());
		
		auto DataTypes = YAMLdata["DataTypes"];
		for (auto dataType : DataTypes) {
			MaterialDataType daty = dataType.as<MaterialDataType>();
			data.push_back(daty);
		}
		Material mat = Material(AssetsManager::get(shaderUuid), data);
		mat.m_uuid = uuid;
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		mat.m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);
		return mat;
	}
	Material::Material(Asset& shader, const std::vector<MaterialDataType>& data) {
		m_shader = std::any_cast<Ref<Shader>>(shader.data);
		m_shaderUuid = shader.uuid;
		m_uuid = UUID();
		update(data);
	}
	void Material::toDataStringFile(std::string path) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << (uint64_t)m_shaderUuid;
		out << YAML::Key << "DataTypes" << YAML::Value << YAML::BeginSeq;
		for (auto& dataType : m_dataTypes) {
			out << dataType;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);
	}
	void Material::bind() {
		if (s_materialBuffer == nullptr)
			return;
		m_shader->bind();
		uint32_t stride = 0;
		for (int i = 0; i < m_dataTypes.size(); i++) {
			MaterialDataType& dataType = m_dataTypes[i];
			//for textures
			Asset asset;
			MaterialTexture texture;

			switch (dataType.type) {
				/*
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
																											
				//case ShaderDataType::Bool:		m_shader->setBool()										break;*/
				case ShaderDataType::Float:
					s_materialBuffer->setData(&std::any_cast<float&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float2:
					s_materialBuffer->setData(&std::any_cast<glm::vec2&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float3:
					s_materialBuffer->setData(&std::any_cast<glm::vec3&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float4:
					s_materialBuffer->setData(&std::any_cast<glm::vec4&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Int:
					s_materialBuffer->setData(&std::any_cast<int&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Mat4:		
					s_materialBuffer->setData(&std::any_cast<glm::mat4&>(dataType.data), dataTypeSize(dataType.type), stride);
					stride += dataTypeSize(dataType.type);
					break;
				case ShaderDataType::Sampler:
					texture = std::any_cast<Stulu::MaterialTexture>(dataType.data);
					asset = AssetsManager::get(texture.texture);
					switch (asset.type)
					{
					case Stulu::AssetType::Texture2D:
						asset.data._Cast<Ref<Texture2D>>()->get()->bind(texture.binding);
						break;
					case Stulu::AssetType::Texture:
						asset.data._Cast<Ref<Texture2D>>()->get()->bind(texture.binding);
						break;
					case Stulu::AssetType::CubeMap:
						asset.data._Cast<Ref<CubeMap>>()->get()->bind(texture.binding);
						break;
					}
					break;
				default:
					CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
			}
		}
	}
	void Material::unbind(){
		m_shader->unbind();
	}
	void Material::update(const std::vector<MaterialDataType>& data) {
		m_dataTypes = data;
		std::vector<MaterialDataType> temp(m_dataTypes.size()); 
		for (auto& d : m_dataTypes) {
			temp[d.order] = d;
		}
		m_dataTypes = temp;
		bind();
	}
	bool Material::operator==(const Material& other) const {
		bool samePath = m_uuid == other.m_uuid;
		bool sameShaderPath = m_shaderUuid == other.m_shaderUuid;
		bool sameData = m_dataTypes == other.m_dataTypes;

		return sameData && sameShaderPath && sameData;
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
			case ShaderDataType::Sampler:	sameData = std::any_cast<MaterialTexture>(data) == std::any_cast<MaterialTexture>(other.data);				break;
		}


		return sameData;

	}
}