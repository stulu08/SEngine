#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	class SharedShaderAssetData : public SharedAssetData {
	public:
		SharedShaderAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedShaderAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}
		SharedShaderAssetData(UUID uuid, const Ref<Shader> shader)
			: SharedAssetData(uuid, MemoryAssetPath), m_shader(shader) {
		}

		virtual std::type_index GetTypeID() const override { return typeid(SharedShaderAssetData); }
		virtual const char* GetTypeName() const override { return "ShaderAsset"; }
		virtual bool Loaded() const override { return  m_shader != nullptr; }
		virtual bool Save() const override { return true; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".glsl" || extension == ".hlsl" || extension == ".comp";
		};
		virtual void Unload() override {
			if (Loaded() && !IsMemoryLoaded()) {
				Renderer::getShaderSystem()->RemoveShader(m_shader->getName());
				m_shader.reset();
			}
		}
		virtual bool Load() override { 
			if (!IsMemoryLoaded()) {
				Renderer::getShaderSystem()->AddShader(GetPath());
			}
			return Loaded();
		}

		using NativeType = Shader;
		NativeType* GetNative() { return m_shader.get(); }
		const NativeType* GetNative() const { return m_shader.get(); }
	private:
		Ref<Shader> m_shader;
	};

	using ShaderAsset = AssetHandle<SharedShaderAssetData>;
}