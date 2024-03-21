#pragma once
namespace StuluBindings {
	class SpriteRendererComponent {
	public:
		static inline void getTiling(uint32_t go, struct Vector2* tiling) {
			auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				tiling->x = spriteRen.tiling.x;
				tiling->y = spriteRen.tiling.y;
			}
		}
		static inline void setTiling(uint32_t go, struct Vector2* tiling) {
			auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				spriteRen.tiling = tiling->toNative();
			}
		}

		static inline void getColor(uint32_t go, struct Vector4* color) {
			auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				color->x = spriteRen.color.x;
				color->y = spriteRen.color.y;
				color->z = spriteRen.color.z;
				color->w = spriteRen.color.w;
			}
		}
		static inline void setColor(uint32_t go, struct Vector4* color) {
			auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				spriteRen.color = color->toNative();
			}
		}

		static inline void setTexture(uint32_t go, uint64_t id) {
			Stulu::Asset asset;
			if (Stulu::AssetsManager::saveGetAndType(id, asset, Stulu::AssetType::Texture2D)) {
				auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
					auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
					spriteRen.texture = std::dynamic_pointer_cast<Stulu::Texture2D>(std::any_cast<Stulu::Ref<Stulu::Texture>>(asset.data));
				}
			}
		}
		static inline uint64_t getTexture(uint32_t go) {
			auto gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				return spriteRen.texture->uuid;
			}
			return Stulu::UUID::null;
		}
	};
}