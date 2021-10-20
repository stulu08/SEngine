#include "st_pch.h"
#include "Material.h"
#include "Stulu/Scene/YAML.h"

namespace Stulu {
	Material Material::fromDataStringPath(const std::string& path) {
		MaterialData data;
		YAML::Node YAMLdata = YAML::LoadFile(path);
		std::string shaderPath = YAMLdata["Shader"].as<std::string>();
		data.materialStructName = YAMLdata["StructName"].as<std::string>();
		
		auto DataTypes = YAMLdata["DataTypes"];
		for (auto dataType : DataTypes) {
			MaterialDataType daty = dataType.as<MaterialDataType>();
			data.dataTypes.push_back(daty);
		}
		Material mat = Material(shaderPath, data);
		mat.m_path = path;
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		mat.m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);
		return mat;
	}
	Material::Material(const std::string& shaderPath, const MaterialData& data)
		: m_shaderPath(shaderPath), m_data(data) {
		m_shader = Shader::create(m_shaderPath);
	}
	void Material::toDataStringFile(std::string path) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << m_shaderPath;
		out << YAML::Key << "StructName" << YAML::Value << m_data.materialStructName;
		out << YAML::Key << "DataTypes" << YAML::Value << YAML::BeginSeq;
		for (auto& dataType : m_data.dataTypes) {
			out << dataType;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);
	}
	void Material::uploadData() {
		m_shader->bind();
		for (auto& dataType : m_data.dataTypes) {
			std::string name = m_data.materialStructName + "." + dataType.name;
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
				default:
					CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
			}
		}
		if(!m_path.empty())
			toDataStringFile(m_path);
	}
	bool Material::operator==(const Material& other) const {
		bool samePath = m_path == other.m_path;
		bool sameShaderPath = m_shaderPath == other.m_shaderPath;
		bool sameDataStructName = m_data.materialStructName == other.m_data.materialStructName;
		bool sameData = m_data.dataTypes == other.m_data.dataTypes;

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
			default:
				CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
		}


		return sameData;

	}
}