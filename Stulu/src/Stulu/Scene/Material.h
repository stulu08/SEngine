#pragma once
#include <Stulu/Renderer/Buffer.h>
#include <Stulu/Renderer/Shader.h>
#include <Stulu/Renderer/Texture.h>
#include <Stulu/Renderer/UniformBuffer.h>
#include "Stulu/Core/UUID.h"
#include <any>

namespace Stulu {
	static uint32_t dataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:		return sizeof(float_t);
		case ShaderDataType::Float2:	return sizeof(glm::vec2);
		case ShaderDataType::Float3:	return sizeof(glm::vec3);
		case ShaderDataType::Float4:	return sizeof(glm::vec4);

		case ShaderDataType::Int:		return sizeof(int32_t);
		case ShaderDataType::Int2:		return sizeof(glm::i32vec2);
		case ShaderDataType::Int3:		return sizeof(glm::i32vec3);
		case ShaderDataType::Int4:		return sizeof(glm::i32vec4);

		case ShaderDataType::Mat3:		return sizeof(glm::mat3);
		case ShaderDataType::Mat4:		return sizeof(glm::mat4);

		case ShaderDataType::Bool:		return sizeof(bool);
		case ShaderDataType::Sampler:	return 4;//uint
		}
		CORE_ERROR("Uknown ShaderDataType");
		return 0;
	}
	struct Asset;
	struct MaterialTexture {
		int binding;
		Ref<Texture> texture;
		int type;//Texture Type 1 = Texture2D | 2 = Texture | 3 = CubeMap 
		UUID uuid;
		bool operator == (const MaterialTexture& other) const {
			return (other.binding == binding && other.texture == texture);
		}
	};
	struct STULU_API MaterialDataType {
		ShaderDataType type;
		std::any data;
		std::string name;
		uint32_t order;
		bool operator == (const MaterialDataType& other) const;
	};
	class STULU_API Material {
	public:
		static Material fromDataStringPath(const std::string& path, UUID uuid);

		Material() {

		}
		Material(Asset& shader, const std::vector<MaterialDataType>& data);

		void toDataStringFile(std::string path);
		std::string toDataString(bool addHelpComments = false);

		void bind();
		void unbind();

		Ref<Shader>& getShader() { return m_shader; }
		std::vector<MaterialDataType>& getData() { return m_dataTypes; }
		UUID& getUUID() { return m_uuid; }
		const std::string& getName() { return m_name; }
		void update(const std::vector<MaterialDataType>& data);


		bool operator == (const Material& other) const;

		//used by Renderer to draw in transaprency render pass
		bool isTransparent = false;
	private:
		UUID m_uuid = UUID::null;
		Ref<Shader> m_shader = nullptr;
		UUID m_shaderUuid = UUID::null;
		std::string m_name = "none";
		std::vector<MaterialDataType> m_dataTypes;
		

		static inline Ref<UniformBuffer> s_materialBuffer = nullptr;

		friend class AssetsManager;
		friend class SceneRenderer;
	};
}