#include "st_pch.h"
#include "Material.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/YAML.h"
#include <Stulu/Core/Resources.h>


namespace Stulu {
	Ref<Material> Material::create(const std::string& path, UUID uuid) {
		std::vector<MaterialDataType> data;
		YAML::Node YAMLdata = YAML::LoadFile(path);
		UUID shaderUuid = UUID(YAMLdata["Shader"].as<uint64_t>());
		
		auto DataTypes = YAMLdata["DataTypes"];
		for (auto dataType : DataTypes) {
			try {
				MaterialDataType daty = dataType.as<MaterialDataType>();
				data.push_back(daty);
			}
			catch (YAML::TypedBadConversion<MaterialDataType> ex) {
				CORE_ASSERT(false, std::string("YAML Error in File " + path + ": " + ex.what()));
			}
		}
		Ref<Material> mat = createRef<Material>(AssetsManager::get(shaderUuid), data);
		mat->m_uuid = uuid;
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		mat->m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);
		if (YAMLdata["transparencyMode"])
			mat->m_transparenceMode = (TransparencyMode)YAMLdata["transparencyMode"].as<uint32_t>();
		if (YAMLdata["alphaCuttOff"])
			mat->m_alphaCutOff = YAMLdata["alphaCuttOff"].as<float>();
		return mat;
	}
	Material::Material(Asset& shader, const std::vector<MaterialDataType>& data, std::string name) {
		m_shader = std::any_cast<Ref<Shader>>(shader.data);
		m_shaderUuid = shader.uuid;
		m_uuid = UUID();
		m_name = name;
		update(data);
	}
	void Material::toDataStringFile(std::string path) {
		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, toDataString().c_str());
		fclose(file);
	}
	std::string Material::toDataString(bool addHelpComments) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << (uint64_t)m_shaderUuid;
		out << YAML::Key << "transparencyMode" << YAML::Value << (uint32_t)m_transparenceMode;
		out << YAML::Key << "alphaCuttOff" << YAML::Value << m_alphaCutOff;
		out << YAML::Key << "DataTypes" << YAML::Value << YAML::BeginSeq;
		for (auto& dataType : m_dataTypes) {
			out << dataType;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::stringstream cs;
		cs << out.c_str();
		if (addHelpComments) {
			cs	<< "\n# How to edit a material:"
				<< "\n# NOTE: Use only Spaces and no tabs, keep an eye on YAML Syntax: https://docs.ansible.com/ansible/latest/reference_appendices/YAMLSyntax.html"
				<< "\n# -----------------------"
				<< "\n# Shader: UUID of Shader"
				<< "\n# -----------------------"
				<< "\n# DataTypes: An list of Datatypes, which you can edit and will be uploaded in a buffer"
				<< "\n# -----------------------"
				<< "\n# How an DataType is build: [ShaderDataType, value, name, index]"
				<< "\n# The Value for a vec4 = [x,y,z,w], vec3 = [x,y,z], vec2 = [x,y]"
				<< "\n# The Value for a Texture is: [binding, Texture UUID, texture type, defaultTexure(white or black)]"
				<< "\n# Texture Types: 1 = Texture | 3 = CubeMap"
				<< "\n# ShaderDataTypes:"
				<< "\n#   1 : Float"
				<< "\n#   2 : Float2/vec2"
				<< "\n#   3 : Float3/vec3"
				<< "\n#   4 : Float4/vec4"
				<< "\n#   5 : int"
				<< "\n#   10: mat4"
				<< "\n#   12: Texture/Sampler";
		}
		return cs.str();
	}
	void Material::uploadData() {
		if (s_materialBuffer == nullptr || uploadDataStart == nullptr){
			CORE_ERROR("Cant upload material data from Material {0}, buffer is null", m_name);
			return;
		}
		
		s_materialBuffer->setData(uploadDataStart, (uint32_t)uploadSize, 0);
		//upload textures
		for (int i = 0; i < m_dataTypes.size(); i++) {
			MaterialDataType& dataType = m_dataTypes[i];
			if (dataType.type == ShaderDataType::Sampler) {
				auto& texture = std::any_cast<Stulu::MaterialTexture&>(dataType.data);
				if (texture.texture != nullptr)
					texture.texture->bind(texture.binding);
				else
					texture.defaultTexture->bind(texture.binding);
			}
		}
		/*
		uint32_t stride = 0;
		s_materialBuffer->setData(&m_transparenceMode, sizeof(uint32_t), 0);
		stride += sizeof(uint32_t);
		s_materialBuffer->setData(&m_alphaCutOff, sizeof(float_t), stride );
		stride += sizeof(float_t)*3;//std140 layout
		for (int i = 0; i < m_dataTypes.size(); i++) {
			MaterialDataType& dataType = m_dataTypes[i];
			MaterialTexture texture;
			switch (dataType.type) {
				case ShaderDataType::Float:
					s_materialBuffer->setData(&std::any_cast<float&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float2:
					s_materialBuffer->setData(&std::any_cast<glm::vec2&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float3:
					s_materialBuffer->setData(&std::any_cast<glm::vec3&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float4:
					s_materialBuffer->setData(&std::any_cast<glm::vec4&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Int:
					s_materialBuffer->setData(&std::any_cast<int&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Mat4:		
					s_materialBuffer->setData(&std::any_cast<glm::mat4&>(dataType.data), getShaderDataTypeSize(dataType.type), stride);
					stride += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Sampler:
					texture = std::any_cast<Stulu::MaterialTexture&>(dataType.data);
					if (texture.texture != nullptr)
						texture.texture->bind(texture.binding);
					else
						texture.defaultTexture->bind(texture.binding);

					break;
				default:
					CORE_ERROR("Uknown ShaderDataType or not supported");
			}
		}
		*/
	}
	void Material::update(const std::vector<MaterialDataType>& data) {
		m_dataTypes = data;
		std::vector<MaterialDataType> temp(m_dataTypes.size());
		for (auto& d : m_dataTypes) {
			if (d.type == ShaderDataType::Sampler) {
				MaterialTexture& tex = std::any_cast<MaterialTexture&>(d.data);
				if (tex.uuid != UUID::null && AssetsManager::exists(tex.uuid)) {
					Asset& asset = Stulu::AssetsManager::get(tex.uuid);
					tex.texture = std::any_cast<Ref<Texture>>(asset.data);
				}
				else {
					tex.texture = nullptr;
				}
			}
			temp[d.order] = d;
		}
		m_dataTypes = temp;
		
		//calculate data size
		{
			uploadSize = 0;

			//transparencyMode and alpaCoutOff
			uploadSize += sizeof(uint32_t);
			uploadSize += sizeof(float_t) * 3;//std140 layout

			//std140 layout alignment
			uint32_t cComponentCount = 0;

			for (int i = 0; i < m_dataTypes.size(); i++) {
				MaterialDataType& dataType = m_dataTypes[i];
				if (cComponentCount > 4 - getShaderDataTypeComponentCount(dataType.type)) {
					uploadSize += sizeof(float_t) * (4 - cComponentCount);
					cComponentCount = 0;
				}
				switch (dataType.type) {
				case ShaderDataType::Float:
					uploadSize += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float2:
					uploadSize += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float3:
					uploadSize += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float4:
					uploadSize += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Int:
					uploadSize += getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Mat4:
					uploadSize += getShaderDataTypeSize(dataType.type);
					//i know that this wont work
					break;
				}
				cComponentCount += getShaderDataTypeComponentCount(dataType.type);

			}
		}
		{
			//put data in uploadbuffer
			if (uploadDataStart)
				free(uploadDataStart);
			uploadDataStart = malloc(uploadSize);
			uint8_t* writePtr = (uint8_t*)uploadDataStart;

			//transparencyMode
			memcpy_s(writePtr, sizeof(uint32_t), &m_transparenceMode, sizeof(uint32_t));
			writePtr = writePtr + sizeof(uint32_t);
			//m_alphaCutOff
			memcpy_s(writePtr, sizeof(float_t), &m_alphaCutOff, sizeof(float_t));
			writePtr = writePtr + sizeof(float_t) * 3;//std140 layout

			uint32_t cComponentCount = 0;
			for (int i = 0; i < m_dataTypes.size(); i++) {
				MaterialDataType& dataType = m_dataTypes[i];
				if (cComponentCount > 4 - getShaderDataTypeComponentCount(dataType.type)) {
					static void* s_emptyBuffer = nullptr;
					if (s_emptyBuffer == nullptr) {
						s_emptyBuffer = new float[4];
					}
					uint32_t size = (sizeof(float_t) * (4 - cComponentCount));
					memcpy_s(writePtr, size, s_emptyBuffer, size);
					writePtr = writePtr + size;
					cComponentCount = 0;
				}
				switch (dataType.type) {
				case ShaderDataType::Float:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<float&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float2:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<glm::vec2&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float3:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<glm::vec3&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Float4:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<glm::vec4&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Int:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<int32_t&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				case ShaderDataType::Mat4:
					memcpy_s(writePtr, getShaderDataTypeSize(dataType.type), &std::any_cast<glm::mat4&>(dataType.data), getShaderDataTypeSize(dataType.type));
					writePtr = writePtr + getShaderDataTypeSize(dataType.type);
					break;
				}
				cComponentCount += getShaderDataTypeComponentCount(dataType.type);
			}
		}
		
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
	std::string DefaultTetxureToString(const Ref<Texture>& texture) {
		if (texture == Resources::getWhiteTexture())
			return "white";
		else if (texture == Resources::getBlackTexture())
			return "black";

		return "white";
	}
	const Ref<Texture> StringToDefaultTexture(const std::string& texture) {
		if (texture == "white")
			return Resources::getWhiteTexture();
		else if (texture == "black")
			return Resources::getBlackTexture();

		return Resources::getBlackTexture();
	}

}