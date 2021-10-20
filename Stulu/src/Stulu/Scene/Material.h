#pragma once
#include <Stulu/Renderer/Buffer.h>
#include <Stulu/Renderer/Shader.h>
#include <any>

namespace Stulu {
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
	class Material {
	public:
		static Material fromDataStringPath(const std::string& path);

		Material() {

		}
		Material(const std::string& shaderPath, const MaterialData& data);

		void toDataStringFile(std::string path);

		void uploadData();

		Ref<Shader>& getShader() { return m_shader; }
		MaterialData& getData() { return m_data; }
		const std::string& getPath() { return m_path; }
		const std::string& getName() { return m_name; }

		bool operator == (const Material& other) const;
	private:
		Ref<Shader> m_shader = nullptr;
		std::string m_shaderPath = "Stulu/assets/Shaders/pbr.glsl";
		std::string m_path = "none";
		std::string m_name = "none";
		MaterialData m_data;

		inline static float* materialStorage = new float[2000];
	};
}