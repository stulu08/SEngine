#pragma once

namespace StuluBindings {
	class Texture2D {
	public:
		static inline float getWidth(uint64_t id) {
			using namespace Stulu;
			Texture2DAsset asset = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(id);
			if (asset.IsLoaded()) return (float)asset->getWidth();
			return 0.0f;
		}
		static inline float getHeight(uint64_t id) {
			using namespace Stulu;
			Texture2DAsset asset = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(id);
			if (asset.IsLoaded()) return (float)asset->getHeight();
			return 0.0f;
		}
		static inline MonoString* getPath(uint64_t id) {
			using namespace Stulu;
			Texture2DAsset asset = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(id);
			if (asset.IsValid() && !asset.IsMemoryLoaded()) 
				return Stulu::Mono::String::New(getCoreDomain(), asset.Path());
			return Stulu::Mono::String::New(getCoreDomain(), "");
		}
		static inline uint64_t findByPath(Stulu::Mono::String mono_path) {
			const std::string path = mono_path.ToUtf8();
			Stulu::SharedAssetData* assetData = Stulu::AssetsManager::GlobalInstance().GetFromPath(Stulu::Resources::AppAssetDir + "/" + path);
			if (assetData) {
				return assetData->GetUUID();
			}
			return Stulu::UUID::null;
		}
		static inline uint64_t getWhite() {
			return Stulu::Resources::UUIDWhiteTexture;
		}
		static inline uint64_t getBlack() {
			return Stulu::Resources::UUIDBlackTexture;
		}
	};
}