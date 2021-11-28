#pragma once
#include <Stulu/Renderer/Buffer.h>
#include <Stulu/Renderer/Shader.h>
#include <Stulu/Renderer/Texture.h>
#include "Stulu/Core/UUID.h"
#include <any>

namespace Stulu {
	struct Asset;
	struct MaterialDataType {
		ShaderDataType type;
		std::any data;
		std::string name;

		bool operator == (const MaterialDataType& other) const;
	};
	struct MaterialData {
		std::vector<MaterialDataType> dataTypes;
		std::string materialStructName;
		MaterialData() = default;
		MaterialData(std::vector<MaterialDataType>& dataTypes)
			: dataTypes(dataTypes){
			materialStructName = "u_mat";
		}
	};
	class STULU_API Material {
	public:
		static Material fromDataStringPath(const std::string& path, UUID uuid);

		Material() {

		}
		Material(Asset& shader, const MaterialData& data);

		void toDataStringFile(std::string path);

		void uploadData();
		void bind();
		void unbind();

		Ref<Shader>& getShader() { return m_shader; }
		MaterialData& getData() { return m_runtimeData; }
		UUID& getUUID() { return m_uuid; }
		const std::string& getName() { return m_name; }

		bool operator == (const Material& other) const;
	private:
		Ref<Shader> m_shader = nullptr;
		UUID m_uuid = UUID::null;
		UUID m_shaderUuid = UUID::null;
		std::string m_name = "none";
		MaterialData m_runtimeData;

		inline static float* materialStorage = new float[2000];
	};
}