#pragma once
#include <Stulu/Renderer/Buffer.h>
#include <Stulu/Renderer/Shader.h>
#include <Stulu/Renderer/Texture.h>
#include <Stulu/Renderer/Buffer.h>
#include <Stulu/Renderer/Renderer.h>
#include "Stulu/Types/UUID.h"
#include <any>

namespace Stulu {
	STULU_API std::string DefaultTetxureToString(const Ref<Texture>& texture);
	STULU_API const Ref<Texture> StringToDefaultTexture(const std::string& texture);

	static uint32_t getShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:		return sizeof(float_t);
		case ShaderDataType::Float2:	return sizeof(glm::vec2);//std140 layout rules, always align to an array of 4 scalars
		case ShaderDataType::Float3:	return sizeof(glm::vec3);//std140 layout rules, always align to an array of 4 scalars
		case ShaderDataType::Float4:	return sizeof(glm::vec4);

		case ShaderDataType::Int:		return sizeof(int32_t);
		case ShaderDataType::Int2:		return sizeof(glm::i32vec2);//std140 layout rules, always align to an array of 4 scalars
		case ShaderDataType::Int3:		return sizeof(glm::i32vec3);//std140 layout rules, always align to an array of 4 scalars
		case ShaderDataType::Int4:		return sizeof(glm::i32vec4);

		case ShaderDataType::Mat3:		return sizeof(glm::mat3);
		case ShaderDataType::Mat4:		return sizeof(glm::mat4);

		case ShaderDataType::Bool:		return sizeof(bool);
		case ShaderDataType::Sampler:	return 0;
		}
		CORE_ERROR("Uknown ShaderDataType");
		return 0;
	}
	static uint32_t getShaderDataTypeComponentCount(ShaderDataType type) {

		switch (type) {
		case ShaderDataType::Float:		return 1;
		case ShaderDataType::Float2:	return 2;
		case ShaderDataType::Float3:	return 3;
		case ShaderDataType::Float4:	return 4;

		case ShaderDataType::Int:		return 1;
		case ShaderDataType::Int2:		return 2;
		case ShaderDataType::Int3:		return 3;
		case ShaderDataType::Int4:		return 4;

		case ShaderDataType::Mat3:		return 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4;

		case ShaderDataType::Bool:		return 1;

		case ShaderDataType::Sampler:	return 0;
		}
		CORE_ERROR("Uknown ShaderDataType: {0}", (int)type);
		return 0;
	}
	struct Asset;
	struct MaterialTexture {
		int binding;
		Ref<Texture> texture;
		Ref<Texture> defaultTexture;
		int type;//Texture Type 1 = Texture | 3 = SkyBox 
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
	enum class TransparencyMode : uint32_t {
		Opaque, Cutout, Transparent
	};
	class STULU_API Material {
	public:
		static Ref<Material> create(const std::string& path, UUID uuid);

		Material(Asset& shader, const std::vector<MaterialDataType>& data, const std::string& name = "new Material", TransparencyMode tMode = TransparencyMode::Opaque, float alphaCuttOff = .5f);
		~Material() {
			if(uploadDataStart)
				free(uploadDataStart);
		}

		void toDataStringFile(std::string path);
		std::string toDataString(bool addHelpComments = false);

		void uploadData();
		void update() {
			update(m_dataTypes);
		}
		void update(const std::vector<MaterialDataType>& data);

		UUID& getUUID() { return m_uuid; }
		Ref<Shader>& getShader() { return m_shader; }
		Ref<ShaderEntry> getShaderEntry() { return Renderer::getShaderSystem()->GetEntry(m_shader->getName()); }
		const std::string& getName() { return m_name; }
		std::vector<MaterialDataType>& getData() { return m_dataTypes; }
		bool isTransparent() { return m_transparenceMode == TransparencyMode::Transparent || m_transparenceMode == TransparencyMode::Cutout; }
		TransparencyMode& getTransparencyMode() { return m_transparenceMode; }
		float& getAlphaCutOff() { return m_alphaCutOff; }

		bool isReadonly = false;
		bool operator == (const Material& other) const;
	private:
		UUID m_uuid = UUID::null;
		Ref<Shader> m_shader = nullptr;
		UUID m_shaderUuid = UUID::null;
		std::string m_name = "none";
		size_t uploadSize = 0;
		void* uploadDataStart = nullptr;
		std::vector<MaterialDataType> m_dataTypes;
		
		TransparencyMode m_transparenceMode;
		float m_alphaCutOff;

		friend class AssetsManager;
		friend class SceneRenderer;
	};
}