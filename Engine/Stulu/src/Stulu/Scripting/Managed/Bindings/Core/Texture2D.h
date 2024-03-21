#pragma once

namespace StuluBindings {
	class Texture2D {
	public:
		static inline float getWidth(uint64_t id) {
			float width = 0.0f;
			using namespace Stulu;
			Stulu::Asset asset;
			if (Stulu::AssetsManager::saveGetAndType(id, asset, Stulu::AssetType::Texture2D)) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(asset.data));
				if (texture) {
					width = (float)texture->getWidth();
				}
			}
			return width;
		}
		static inline float getHeight(uint64_t id) {
			using namespace Stulu;
			float height = 0.0f;
			Stulu::Asset asset;
			if (Stulu::AssetsManager::saveGetAndType(id, asset, Stulu::AssetType::Texture2D)) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(asset.data));
				if (texture) {
					height = (float)texture->getHeight();
				}
			}
			return height;
		}
		static inline MonoString* getPath(uint64_t id) {
			using namespace Stulu;

			std::string path = "";
			Stulu::Asset asset;
			if (Stulu::AssetsManager::saveGetAndType(id, asset, Stulu::AssetType::Texture2D)) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(asset.data));
				if (texture) {
					path = texture->getPath();
				}
			}
			return mono_string_new(getCoreDomain(), path.c_str());
		}
		static inline uint64_t findByPath(MonoString* mono_path) {
			const std::string path = mono_string_to_utf8(mono_path);
			Stulu::UUID uuid = Stulu::AssetsManager::getFromPath((Stulu::Resources::AppAssetDir + "/" + path), Stulu::AssetType::Texture2D);
			return uuid;
		}
		static inline uint64_t getWhite() {
			return Stulu::Resources::getWhiteTexture()->uuid;
		}
		static inline uint64_t getBlack() {
			return Stulu::Resources::getBlackTexture()->uuid;
		}
	};
}