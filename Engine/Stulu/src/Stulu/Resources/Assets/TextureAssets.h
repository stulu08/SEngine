#pragma once
#include "Stulu/Resources/AssetData.h"
#include "Stulu/Resources/AssetHandel.h"
#include "Stulu/Renderer/Texture.h"
#include <magic_enum/magic_enum.hpp>

namespace Stulu {
	class STULU_API SharedTexture2DAssetData : public SharedAssetData {
	public:
		SharedTexture2DAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}

		SharedTexture2DAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}
		SharedTexture2DAssetData(UUID uuid, Ref<Texture2D> texture)
			: SharedAssetData(uuid, MemoryAssetPath), m_texture(texture) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedTexture2DAssetData); }
		virtual const char* GetTypeName() const override { return "Texture2DAsset"; }
		virtual void Unload() override { m_texture.reset(); }
		virtual bool Loaded() const override { return m_texture != nullptr; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga";
		};

		TextureSettings ReadSettings();
		bool SaveSetting(TextureSettings settings) const;

		virtual bool Load() override;
		virtual bool Save() const override;

		Ref<Texture2D> GetTextureHandle() const {
			return m_texture;
		}

		using NativeType = Texture2D;
		NativeType* GetNative() { return m_texture.get(); }
		const NativeType* GetNative() const { return m_texture.get(); }
	private:
		Ref<Texture2D> m_texture;
	};

	class STULU_API SharedSkyBoxAssetData : public SharedAssetData {
	public:
		SharedSkyBoxAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}

		SharedSkyBoxAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedSkyBoxAssetData); }
		virtual const char* GetTypeName() const override { return "SkyBoxAsset"; }
		virtual void Unload() override { m_texture.reset(); }
		virtual bool Loaded() const override { return m_texture != nullptr; }
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".hdr" || extension == ".skybox" || extension == ".tif";
		};
		virtual bool Load() override;
		virtual bool Save() const override;

		using NativeType = SkyBox;
		NativeType* GetNative() { return m_texture.get(); }
		const NativeType* GetNative() const { return m_texture.get(); }
	private:
		Ref<SkyBox> m_texture;
	};

	class SharedRenderTextureAssetData : public SharedAssetData {
	public:
		SharedRenderTextureAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}

		SharedRenderTextureAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

		virtual std::type_index GetTypeID() const override { return typeid(SharedRenderTextureAssetData); }
		virtual const char* GetTypeName() const override { return "RenderTextureAsset"; }
		virtual void Unload() override { m_texture.reset(); }
		virtual bool Loaded() const override { return m_texture != nullptr; }
		virtual bool Load() override { return true; }
		virtual bool Save() const override { return true; }
		virtual bool HasExtension(const std::string& extension) const override { return extension == ".srt"; };

		void SetSource(const Ref<Texture2D>& source) {
			m_texture = source;
		}
		Ref<Texture2D> GetTextureHandle() const {
			return m_texture;
		}

		using NativeType = Texture2D;
		NativeType* GetNative() { return m_texture.get(); }
		const NativeType* GetNative() const { return m_texture.get(); }
	private:
		Ref<Texture2D> m_texture;
	};

	using Texture2DAsset = AssetHandle<SharedTexture2DAssetData>;
	using SkyBoxAsset = AssetHandle<SharedSkyBoxAssetData>;
	using RenderTextureAsset = AssetHandle<SharedRenderTextureAssetData>;

	template<typename T>
	struct TextureAssetMap {
		using type = Texture2DAsset;
	};
	template<>
	struct TextureAssetMap<Texture2D> {
		using type = Texture2DAsset;
	};
	template<>
	struct TextureAssetMap<SkyBox> {
		using type = SkyBoxAsset;
	};
	template<typename T>
	using TextureAsset = typename TextureAssetMap<T>::type;
}