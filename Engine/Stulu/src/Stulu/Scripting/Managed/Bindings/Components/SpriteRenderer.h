#pragma once
namespace StuluBindings {
	class SpriteRendererComponent {
	public:
		static inline void getTiling(uint64_t go, struct Vector2* tiling) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				tiling->x = spriteRen.tiling.x;
				tiling->y = spriteRen.tiling.y;
			}
		}
		static inline void setTiling(uint64_t go, struct Vector2* tiling) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				spriteRen.tiling = tiling->toNative();
			}
		}

		static inline void getColor(uint64_t go, struct Vector4* color) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				color->x = spriteRen.color.x;
				color->y = spriteRen.color.y;
				color->z = spriteRen.color.z;
				color->w = spriteRen.color.w;
			}
		}
		static inline void setColor(uint64_t go, struct Vector4* color) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				spriteRen.color = color->toNative();
			}
		}

		static inline void setTexture(uint64_t go, uint64_t id) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				spriteRen.texture = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::Texture2DAsset>(id);
			}
		}
		static inline uint64_t getTexture(uint64_t go) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::SpriteRendererComponent>()) {
				auto& spriteRen = gameObject.getComponent<Stulu::SpriteRendererComponent>();
				return spriteRen.texture.GetUUID();
			}
			return Stulu::UUID::null;
		}
	};
}