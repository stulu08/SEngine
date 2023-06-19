#pragma once
#include "Stulu/Scene/AssetsManager.h"
#include <mono/metadata/object.h>

namespace StuluBindings {
	class Texture2D {
	public:
		static inline float getWidth(uint64_t id) {
			float width = 0.0f;
			using namespace Stulu;
			Stulu::UUID uuid = Stulu::UUID(id);
			if (uuid != Stulu::UUID::null) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data));
				if (texture) {
					width = (float)texture->getWidth();
				}
			}
			return width;
		}
		static inline float getHeight(uint64_t id) {
			using namespace Stulu;
			float height = 0.0f;
			Stulu::UUID uuid = Stulu::UUID(id);
			if (uuid != Stulu::UUID::null) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data));
				if (texture) {
					height = (float)texture->getHeight();
				}
			}
			return height;
		}
		static inline MonoString* getPath(uint64_t id) {
			using namespace Stulu;

			std::string path = "";
			Stulu::UUID uuid = Stulu::UUID(id);

			if (uuid != Stulu::UUID::null) {
				Ref<Stulu::Texture2D> texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data));
				if (texture) {
					path = texture->getPath();
				}
			}

			return mono_string_new(getCoreDomain(), path.c_str());
		}
		static inline uint64_t findByPath(MonoString* mono_path) {
			const std::string path = mono_string_to_utf8(mono_path);
			Stulu::UUID uuid = Stulu::AssetsManager::getFromPath((Stulu::Resources::GameAssetDirectory + "/" + path), Stulu::AssetType::Texture2D);
			return uuid;
		}
	};
}