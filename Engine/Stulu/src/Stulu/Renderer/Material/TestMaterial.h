#pragma once
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Renderer/Shader.h"

namespace Stulu {
	class STULU_API MaterialProperty;
	class STULU_API ShaderEntry;
	class STULU_API Shader;

	enum class MaterialTransparencyMode : uint8_t {
		Opaque = 0,
		Cutout = 1,
		Transparent = 2,
	};

	class STULU_API TestMaterial {
	public:
		TestMaterial(Ref<ShaderEntry> shaderEntry);

		void SetData(size_t offset, size_t size, const void* data);

		// will bind everything beside the shader
		void RenderReady();

		Ref<Shader> GetShader() const { return m_shader; }
		Ref<UniformBuffer> GetBuffer() const { return m_buffer; }

		bool IsTransparent() const { return m_transparencyMode != MaterialTransparencyMode::Opaque; }

		MaterialTransparencyMode GetTransparencyMode() const { return m_transparencyMode; }
		void SetTransparencyMode(MaterialTransparencyMode mode) { m_transparencyMode = mode; }
		float GetAlphaCutOut() const { return m_cutOut; }
		void SetAlphaCutOut(float cutOut) { m_cutOut = cutOut; }
		bool GetInstanced() const { return m_instanced; }
		void SetInstanced(bool instanced) { m_instanced = instanced; }

		bool HasProperity(const std::string& name) const;
		std::vector<Ref<MaterialProperty>>& GetProperities() { return m_properties; }
		const std::vector<Ref<MaterialProperty>>& GetProperities() const { return m_properties; }
		Ref<MaterialProperty> GetProperity(const std::string& name) const;

		template<class T>
		inline Ref<T> GetProperityAs(const std::string& name) const {
			return std::dynamic_pointer_cast<T>(GetProperity(name));
		}
		template<class T>
		inline bool SetAndApplyPropertiy(const std::string& name, const typename T::NativeType& value);

	private:
		Ref<UniformBuffer> m_buffer;
		Ref<Shader> m_shader;
		std::vector<Ref<MaterialProperty>> m_properties;

		MaterialTransparencyMode m_transparencyMode = MaterialTransparencyMode::Opaque;
		float m_cutOut = 0.0f;
		bool m_instanced = false;
	};
}